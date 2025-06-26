import cv2
import numpy as np
import csv
from typing import Tuple, List, Optional


class BallDetector:
    """
    A class for detecting billiard balls in images and converting their positions 
    from image coordinates to world coordinates using camera calibration data.
    """
    
    def __init__(self, intrinsic_path: str, translation_path: str, rotation_path: str):
        """
        Initialize the BallDetector with camera calibration data.
        Args:
            intrinsic_path: Path to the intrinsic matrix CSV file
            translation_path: Path to the translation vector CSV file
            rotation_path: Path to the rotation matrix CSV file
        """
        self.intrinsic_matrix = self._load_csv_matrix(intrinsic_path)
        self.translation_vector = self._load_csv_matrix(translation_path)
        self.rotation_matrix = self._load_csv_matrix(rotation_path)
        self.transformation_matrix = self._build_transformation_matrix()
        self.inverse_transformation = np.linalg.inv(self.transformation_matrix)
    
    def _load_csv_matrix(self, filepath: str) -> np.ndarray:
        """Load matrix data from CSV file."""
        with open(filepath, 'r') as file:
            data = list(csv.reader(file, delimiter=","))
        return np.array(data, dtype=float)
    
    def _build_transformation_matrix(self) -> np.ndarray:
        """Build the transformation matrix from rotation matrix and translation vector."""
        extrinsic_matrix = np.zeros((3, 3))
        
        # Fill rotation part
        extrinsic_matrix[0:3, 0:2] = self.rotation_matrix[0:3, 0:2]
        
        # Fill translation part
        extrinsic_matrix[0:3, 2] = self.translation_vector[0:3, 0]
        
        # Compute H = K * [R|t]
        return np.matmul(self.intrinsic_matrix, extrinsic_matrix)
    
    def preprocess_image(self, image: np.ndarray, contrast: int = 50, 
                        brightness: int = 130, resize_factor: float = 0.5) -> np.ndarray:
        """
        Preprocess the input image by resizing and adjusting contrast/brightness.
        
        Args:
            image: Input image
            contrast: Contrast adjustment value
            brightness: Brightness adjustment value
            resize_factor: Factor to resize the image
            
        Returns:
            Preprocessed image
        """
        # Resize image
        resized = cv2.resize(image, (0, 0), fx=resize_factor, fy=resize_factor)
        
        # Adjust contrast and brightness
        adjusted = resized * (contrast/127 + 1) - contrast + brightness
        adjusted = np.clip(adjusted, 0, 255)
        
        return adjusted.astype(np.uint8)
    
    def detect_circles(self, image: np.ndarray, min_radius: int = 10, 
                      max_radius: int = 18) -> Optional[np.ndarray]:
        """
        Detect circles in the image using HoughCircles.
        
        Args:
            image: Input image
            min_radius: Minimum circle radius
            max_radius: Maximum circle radius
            
        Returns:
            Array of detected circles [x, y, radius] or None if no circles found
        """
        # Convert to grayscale and apply Gaussian blur
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray, (5, 5), 0)
        
        # Detect circles using HoughCircles
        circles = cv2.HoughCircles(
            gray, cv2.HOUGH_GRADIENT, 1, 20, 
            param1=100, param2=15, 
            minRadius=min_radius, maxRadius=max_radius
        )
        
        if circles is not None:
            return circles.astype(int)
        return None
    
    def filter_circles_by_region(self, circles: np.ndarray, 
                                x_min: int = 75, x_max: int = 725,
                                y_min: int = 170, y_max: int = 450) -> np.ndarray:
        """
        Filter circles to keep only those within specified region.
        
        Args:
            circles: Array of detected circles
            x_min, x_max, y_min, y_max: Region boundaries
            
        Returns:
            Filtered circles within the specified region
        """
        valid_circles = []
        
        for x, y, r in circles[0]:
            if x_min <= x <= x_max and y_min <= y <= y_max:
                valid_circles.append([x, y, r])
        
        return np.array(valid_circles) if valid_circles else np.array([])
    
    def identify_cue_ball(self, image: np.ndarray, circles: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """
        Identify the cue ball (white ball) based on color analysis.
        
        Args:
            image: Input image
            circles: Array of detected circles
            
        Returns:
            Tuple of (cue_ball_position, other_balls_positions)
        """
        cue_ball = None
        other_balls = []
        
        for x, y, r in circles:
            is_white = True
            
            # Check if the ball is white by sampling pixels around the center
            for h in range(5, 10):
                positions = [(y+h, x+h), (y-h, x-h), (y+h, x-h), (y-h, x+h)]
                
                for pos_y, pos_x in positions:
                    if (pos_y < 0 or pos_y >= image.shape[0] or 
                        pos_x < 0 or pos_x >= image.shape[1]):
                        is_white = False
                        break
                    
                    pixel = image[pos_y, pos_x]
                    if not (pixel[0] > 253 and pixel[1] > 253 and pixel[2] > 253):
                        is_white = False
                        break
                
                if not is_white:
                    break
            
            if is_white:
                cue_ball = np.array([x, y])
            else:
                other_balls.append([x, y])
        
        return cue_ball, np.array(other_balls)
    
    def scale_coordinates(self, coordinates: np.ndarray, scale_factor: float = 2.0) -> np.ndarray:
        """
        Scale coordinates back to original image size.
        
        Args:
            coordinates: Array of coordinates to scale
            scale_factor: Scaling factor
            
        Returns:
            Scaled coordinates
        """
        return coordinates * scale_factor
    
    def _compute_scale_factor(self, u: float, v: float) -> float:
        """Compute the scale factor for homogeneous coordinate transformation."""
        return (self.inverse_transformation[2, 0] * u + 
                self.inverse_transformation[2, 1] * v + 
                self.inverse_transformation[2, 2])
    
    def image_to_world_coordinates(self, image_points: np.ndarray) -> np.ndarray:
        """
        Convert image coordinates to world coordinates.
        
        Args:
            image_points: Array of image coordinates [x, y]
            
        Returns:
            Array of world coordinates [X, Y]
        """
        world_coords = []
        
        for point in image_points:
            # Convert to homogeneous coordinates
            homogeneous_point = np.array([point[0], point[1], 1])
            
            # Transform to world coordinates
            scale = self._compute_scale_factor(point[0], point[1])
            world_point = np.matmul(self.inverse_transformation, homogeneous_point) / scale
            
            # Convert back to 2D coordinates
            world_coords.append([world_point[0], world_point[1]])
        
        return np.array(world_coords)
    
    def draw_detection_results(self, image: np.ndarray, cue_ball: np.ndarray, 
                              other_balls: np.ndarray, circles: np.ndarray) -> np.ndarray:
        """
        Draw detection results on the image.
        
        Args:
            image: Input image
            cue_ball: Cue ball position
            other_balls: Other balls positions
            circles: All detected circles
            
        Returns:
            Image with drawn detection results
        """
        result_image = image.copy()
        
        # Draw all valid circles in blue
        for x, y, r in circles:
            cv2.circle(result_image, (x, y), r, (255, 0, 0), 2)
        
        # Highlight cue ball in red if found
        if cue_ball is not None:
            # Find the radius of the cue ball
            cue_radius = None
            for x, y, r in circles:
                if x == cue_ball[0] and y == cue_ball[1]:
                    cue_radius = r
                    break
            
            if cue_radius:
                cv2.circle(result_image, tuple(cue_ball), cue_radius, (0, 0, 255), 2)
        
        return result_image
    
    def save_results(self, cue_ball_world: np.ndarray, other_balls_world: np.ndarray, 
                    ball_count: int, output_dir: str = "./"):
        """
        Save detection results to CSV files.
        
        Args:
            cue_ball_world: Cue ball world coordinates
            other_balls_world: Other balls world coordinates
            ball_count: Number of detected balls
            output_dir: Output directory for CSV files
        """
        # Save cue ball coordinates
        np.savetxt(f'{output_dir}mother_Wcoor.csv', 
                  cue_ball_world.reshape(1, -1), delimiter=',', fmt='%f')
        
        # Save other balls coordinates
        if len(other_balls_world) > 0:
            np.savetxt(f'{output_dir}son_Wcoor.csv', 
                      other_balls_world, delimiter=',', fmt='%f')
        
        # Save ball count
        np.savetxt(f'{output_dir}childball_num.csv', 
                  np.array([[ball_count]]), delimiter=',', fmt='%d')


class BallDetectionApp:
    """Main application class for ball detection."""
    
    def __init__(self, intrinsic_path: str, translation_path: str, rotation_path: str):
        """Initialize the application with calibration data paths."""
        self.detector = BallDetector(intrinsic_path, translation_path, rotation_path)
    
    def process_image(self, image_path: str, show_results: bool = True) -> dict:
        """
        Process a single image and detect balls.
        
        Args:
            image_path: Path to the input image
            show_results: Whether to display the results
            
        Returns:
            Dictionary containing detection results
        """
        # Load and preprocess image
        image = cv2.imread(image_path)
        if image is None:
            raise ValueError(f"Could not load image from {image_path}")
        
        processed_image = self.detector.preprocess_image(image)
        
        if show_results:
            cv2.imshow('Preprocessed Image', processed_image)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        
        # Detect circles
        circles = self.detector.detect_circles(processed_image)
        if circles is None:
            print("No circles detected in the image")
            return {'cue_ball': None, 'other_balls': [], 'ball_count': 0}
        
        # Filter circles by region
        valid_circles = self.detector.filter_circles_by_region(circles)
        if len(valid_circles) == 0:
            print("No valid circles found in the specified region")
            return {'cue_ball': None, 'other_balls': [], 'ball_count': 0}
        
        # Identify cue ball and other balls
        cue_ball, other_balls = self.detector.identify_cue_ball(processed_image, valid_circles)
        
        # Scale coordinates back to original size
        if cue_ball is not None:
            cue_ball = self.detector.scale_coordinates(cue_ball)
        if len(other_balls) > 0:
            other_balls = self.detector.scale_coordinates(other_balls)
        
        # Convert to world coordinates
        cue_ball_world = None
        other_balls_world = np.array([])
        
        if cue_ball is not None:
            cue_ball_world = self.detector.image_to_world_coordinates(cue_ball.reshape(1, -1))[0]
        
        if len(other_balls) > 0:
            other_balls_world = self.detector.image_to_world_coordinates(other_balls)
        
        # Draw results
        if show_results:
            result_image = self.detector.draw_detection_results(
                processed_image, 
                cue_ball // 2 if cue_ball is not None else None,  # Scale back for display
                other_balls // 2 if len(other_balls) > 0 else np.array([]),  # Scale back for display
                valid_circles
            )
            cv2.imshow('Detection Results', result_image)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        
        # Print results
        print('Cue Ball (Mother Ball):')
        print(cue_ball_world if cue_ball_world is not None else 'Not detected')
        print('Other Balls (Child Balls):')
        print(other_balls_world if len(other_balls_world) > 0 else 'None detected')
        print('----------------------------------------')
        
        # Save results
        ball_count = len(other_balls) if len(other_balls) > 0 else 0
        self.detector.save_results(cue_ball_world, other_balls_world, ball_count)
        
        return {
            'cue_ball': cue_ball_world,
            'other_balls': other_balls_world,
            'ball_count': ball_count
        }


def main():
    """Main function to run the ball detection application."""
    # Configuration paths
    image_path = r"C:\Users\wang8\OneDrive\桌面\機械手臂相機\ball\balltest3_8-1.bmp"
    intrinsic_path = "C:/Users/samuel901213/Desktop/opencv/IntrinsicMatrix.csv"
    translation_path = "C:/Users/samuel901213/Desktop/opencv/TranslationVectors.csv"
    rotation_path = "C:/Users/samuel901213/Desktop/opencv/RotationMatrices.csv"
    
    try:
        # Initialize and run the application
        app = BallDetectionApp(intrinsic_path, translation_path, rotation_path)
        results = app.process_image(image_path, show_results=True)
        
        print("Detection completed successfully!")
        print(f"Detected {results['ball_count']} balls (excluding cue ball)")
        
    except Exception as e:
        print(f"Error during processing: {e}")


if __name__ == "__main__":
    main()
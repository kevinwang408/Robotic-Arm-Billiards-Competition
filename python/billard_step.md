從main開始應該較好理解
354 def main
    輸入四個path:圖像的路徑、內參路徑、位移量路徑、旋轉矩陣路徑
    執行 ballDetectionApp
    結果=app.process_image

263 BalldetectionApp 
266 init 先將self.detector=BallDetector(三路徑)，即BalldetectionApp.detector
'''''''''''''''''''''''''''''''''''''''''''''''
以下包含在 class detector中
7   BallDetector:用以檢測圖像中的撞球並轉換至世界座標
13  init 讀取三個路徑資料
24  將讀取的路徑資料合併成一格transformation matrix
25  transformation matrix 的inverse
27  載入matrix的函數，就只是讀取而已，
補充 _load_csv_matrix(self,filpath:str)->
指輸入什麼引數後會return甚麼
想要在class中引用class 內自建的函數
要再加一個引數(self)
這個方法是用來將此def的function與class綁在一起
假如我在其他程式碼使用時
detector = BallDetector(...)  # 建立一個實例
matrix = detector._load_csv_matrix("some.csv")  # 呼叫內部方法
這時self都不用打
46  影像處理 resize和adjust contrast
    return 處理後的image
69  圓形檢測
97  邊界條件
118 找出母球
160 scale 座標
173 計算scale的factor
179 座標轉換
204 劃出檢測結果
238 儲存結果

'''''''''''''''''''''''''''''''''''''''''''''
263 class BallDetectionApp
266 設定為BallDetector的class
270 使用 BallDetector的韓式
282 讀取圖片
286 影像處理
288 show
294 圓形檢測
300 邊界條件
306 母球
309 將處理後座標轉換回去
318 圖形座標轉至世界座標
'''''''''''''''''''''''''''''''''''''''''''''''
354 main

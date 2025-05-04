
# Bài tập lớn lập trình nâng cao
- Họ và tên: Nguyễn Công Minh. <br>
- Mã số sinh viên: 24022817. <br> 
- Tên đề tài: Trò chơi nhịp điệu theo nhạc (Drum it!). <br>
- Video demo sản phẩm: https://www.youtube.com/watch?v=Gy2TiDo9upw . <br>
---

## Mô tả trò chơi
- Drum It! là tựa game theo nhạc dựa trên trò chơi thùng Nhật Bản nổi tiếng "Taiko no Tatsujin". Người chơi sẽ phải sử dụng nút để căn thời gian chuẩn cho từng nốt. tuỳ vào độ chính xác của mỗi lần đanh nốt, người
  chơi sẽ nhậnd dược số điểm tương ứng. Mục tiêu của trò chơi là vượt qua được hết bài nhạc và đạt số điểm cao nhất có thể.

## Cách chơi

### Bắt đầu trò chơi:
- Khi khởi động, người chơi sẽ được đưa đến menu chính với các nút: Play và Quit.<br>
- Nhấn Play để vào màn hình chọn cấp độ.<br>
- Chọn một cấp độ (Easy, Normal, Hard, Impossible) để bắt đầu chơi.

### Trong khi chơi:
- Các nốt nhạc sẽ di chuyển từ phải sang trái.<br>
- Sử dụng các phím:
  - F: Đánh nốt màu cam.<br>
  - J: Đánh nốt màu xanh.<br>
- Đánh đúng nốt khi nó nằm trong vùng "hitbox" để ghi điểm.<br>
- Nếu đánh sai hoặc bỏ lỡ nốt thì sẽ mất máu.

### Tạm dừng và kết thúc:
- Nhấn nút Pause để tạm dừng trò chơi.<br>
- Khi bài hát kết thúc hoặc máu về 0 thì sẽ hiển thị màn hình kết thúc với:
  - Điểm số
  - Thống kê chi tiết

---

## Cách tính điểm

### Độ chính xác:
- Excellent: +300 điểm<br>
- Great: +100 điểm<br>
- Ok: +50 điểm<br>
- Missed: 0 điểm và mất máu

### Combo multiplier:
- Mỗi nốt đánh đúng → tăng hệ số nhân điểm (combo).<br>
- Nếu bỏ lỡ hoặc đánh sai sẽ reset combo về 1.

### Máu:
- Người chơi bắt đầu với một lượng máu cố định.<br>
- Mỗi nốt bỏ lỡ hoặc đánh sai thì sẽ giảm máu.<br>
- Trò chơi kết thúc nếu máu giảm về 0.

---

## Giải thích code

### Cấu trúc code:

- File chính: main.cpp. <br>
- Build script: Makefile. <br>
- ./assets: lưu trữ hình ảnh, đồ hoạ cho game. <br>
- ./sfx: lưu trữ âm thành nền và các hiệu ứng. <br>
- ./src: file của thư viện SDL2 và Nholmann: Json.<br>
- ./font: phông chữ. <br>
- ./source: gồm các thành phần file header (.hpp) và file mã nguồn (.cpp), phục vụ việc chia nhỏ chương trình một cách hợp lý. <br>
  - audio: quản lý việc load âm thanh. <br>
  - graphic: quản lý việc load các ảnh, chữ và animation. <br>
  - logic: logic chính của gameplay, quản lý tất cả các cơ chế liên quan đến note (load, tạo, di chuyển, tính điểm, ...)
  - constants: các hằng số của game như vị trí, kích thước của ảnh,... <br>
  - menu, levels, game_display: là các màn hình của game. <br>
- ./beatmaps: lưu trữ các dữ liệu về từng bài nhạc. một beatmap là một thư mục gồm có: background.png, song.mp3 và 1 file data.json chứa thông tin về các tốc độ di chuyển của nốt, số máu và đặc biệt là dữ liệu của tất cả các nốt.<br>

### Các cơ chế quan trọng của game:

- Cơ chế sinh nốt: Trong file data.json, Một note_list là một mãng lưu trữ các mảng con gồm hai phần tử type và hit_time. Sử dụng thư viện Nholmann: Json, chương trình sẽ đọc file đó ra và nạp vào game. Để tính thời gian xuất hiện,
thời gian sinh ra của note sẽ được tính theo hit_time - time_needed (thời gian cần để di chuyển tới hitbox). <br>
- Cơ chế tính điểm: Khi một note tới gần hitbox và trong một khoảng cách nhất định, nếu người chơi nhấn đúng nút mà để đánh nốt thì điểm sẽ được tính bằng độ trễ giữa thời điểm nhấn và hit_time từ data. Dựa vào đó,
điểm sẽ được cộng cho người chơi theo hệ số multiplier. <br>
- Các ảnh, âm thanh, hình ảnh đều được làm trong một cấu trúc class.

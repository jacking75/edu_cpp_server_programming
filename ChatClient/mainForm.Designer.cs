namespace csharp_test_client
{
    partial class mainForm
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnDisconnect = new System.Windows.Forms.Button();
            this.btnConnect = new System.Windows.Forms.Button();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.textBoxPort = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.checkBoxLocalHostIP = new System.Windows.Forms.CheckBox();
            this.textBoxIP = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.textSendText = new System.Windows.Forms.TextBox();
            this.labelStatus = new System.Windows.Forms.Label();
            this.listBoxLog = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxUserID = new System.Windows.Forms.TextBox();
            this.textBoxUserPW = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.Room = new System.Windows.Forms.GroupBox();
            this.btnMatching = new System.Windows.Forms.Button();
            this.btnRoomChat = new System.Windows.Forms.Button();
            this.textBoxRoomSendMsg = new System.Windows.Forms.TextBox();
            this.listBoxRoomChatMsg = new System.Windows.Forms.ListBox();
            this.label4 = new System.Windows.Forms.Label();
            this.listBoxRoomUserList = new System.Windows.Forms.ListBox();
            this.btn_RoomLeave = new System.Windows.Forms.Button();
            this.btn_RoomEnter = new System.Windows.Forms.Button();
            this.textBoxRoomNumber = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button5 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.listBoxLobby = new System.Windows.Forms.ListBox();
            this.xPosTextNumber = new System.Windows.Forms.TextBox();
            this.yPosTextNumber = new System.Windows.Forms.TextBox();
            this.PutStoneBtn = new System.Windows.Forms.Button();
            this.GameStartBtn = new System.Windows.Forms.Button();
            this.groupBox5.SuspendLayout();
            this.Room.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnDisconnect
            // 
            this.btnDisconnect.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnDisconnect.Location = new System.Drawing.Point(601, 66);
            this.btnDisconnect.Margin = new System.Windows.Forms.Padding(4);
            this.btnDisconnect.Name = "btnDisconnect";
            this.btnDisconnect.Size = new System.Drawing.Size(126, 39);
            this.btnDisconnect.TabIndex = 29;
            this.btnDisconnect.Text = "접속 끊기";
            this.btnDisconnect.UseVisualStyleBackColor = true;
            this.btnDisconnect.Click += new System.EventHandler(this.btnDisconnect_Click);
            // 
            // btnConnect
            // 
            this.btnConnect.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnConnect.Location = new System.Drawing.Point(600, 24);
            this.btnConnect.Margin = new System.Windows.Forms.Padding(4);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(126, 39);
            this.btnConnect.TabIndex = 28;
            this.btnConnect.Text = "접속하기";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.textBoxPort);
            this.groupBox5.Controls.Add(this.label10);
            this.groupBox5.Controls.Add(this.checkBoxLocalHostIP);
            this.groupBox5.Controls.Add(this.textBoxIP);
            this.groupBox5.Controls.Add(this.label9);
            this.groupBox5.Location = new System.Drawing.Point(17, 18);
            this.groupBox5.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox5.Size = new System.Drawing.Size(576, 78);
            this.groupBox5.TabIndex = 27;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Socket 더미 클라이언트 설정";
            // 
            // textBoxPort
            // 
            this.textBoxPort.Location = new System.Drawing.Point(321, 30);
            this.textBoxPort.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxPort.MaxLength = 6;
            this.textBoxPort.Name = "textBoxPort";
            this.textBoxPort.Size = new System.Drawing.Size(71, 28);
            this.textBoxPort.TabIndex = 18;
            this.textBoxPort.Text = "11021";
            this.textBoxPort.WordWrap = false;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(233, 36);
            this.label10.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(92, 18);
            this.label10.TabIndex = 17;
            this.label10.Text = "포트 번호:";
            // 
            // checkBoxLocalHostIP
            // 
            this.checkBoxLocalHostIP.AutoSize = true;
            this.checkBoxLocalHostIP.Checked = true;
            this.checkBoxLocalHostIP.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxLocalHostIP.Location = new System.Drawing.Point(407, 36);
            this.checkBoxLocalHostIP.Margin = new System.Windows.Forms.Padding(4);
            this.checkBoxLocalHostIP.Name = "checkBoxLocalHostIP";
            this.checkBoxLocalHostIP.Size = new System.Drawing.Size(149, 22);
            this.checkBoxLocalHostIP.TabIndex = 15;
            this.checkBoxLocalHostIP.Text = "localhost 사용";
            this.checkBoxLocalHostIP.UseVisualStyleBackColor = true;
            // 
            // textBoxIP
            // 
            this.textBoxIP.Location = new System.Drawing.Point(97, 28);
            this.textBoxIP.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxIP.MaxLength = 6;
            this.textBoxIP.Name = "textBoxIP";
            this.textBoxIP.Size = new System.Drawing.Size(123, 28);
            this.textBoxIP.TabIndex = 11;
            this.textBoxIP.Text = "0.0.0.0";
            this.textBoxIP.WordWrap = false;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(9, 34);
            this.label9.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(92, 18);
            this.label9.TabIndex = 10;
            this.label9.Text = "서버 주소:";
            // 
            // button1
            // 
            this.button1.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button1.Location = new System.Drawing.Point(456, 105);
            this.button1.Margin = new System.Windows.Forms.Padding(4);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(143, 39);
            this.button1.TabIndex = 39;
            this.button1.Text = "echo 보내기";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // textSendText
            // 
            this.textSendText.Location = new System.Drawing.Point(17, 111);
            this.textSendText.Margin = new System.Windows.Forms.Padding(4);
            this.textSendText.MaxLength = 32;
            this.textSendText.Name = "textSendText";
            this.textSendText.Size = new System.Drawing.Size(428, 28);
            this.textSendText.TabIndex = 38;
            this.textSendText.Text = "test1";
            this.textSendText.WordWrap = false;
            // 
            // labelStatus
            // 
            this.labelStatus.AutoSize = true;
            this.labelStatus.Location = new System.Drawing.Point(16, 1053);
            this.labelStatus.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.Size = new System.Drawing.Size(173, 18);
            this.labelStatus.TabIndex = 40;
            this.labelStatus.Text = "서버 접속 상태: ???";
            // 
            // listBoxLog
            // 
            this.listBoxLog.FormattingEnabled = true;
            this.listBoxLog.HorizontalScrollbar = true;
            this.listBoxLog.ItemHeight = 18;
            this.listBoxLog.Location = new System.Drawing.Point(14, 810);
            this.listBoxLog.Margin = new System.Windows.Forms.Padding(4);
            this.listBoxLog.Name = "listBoxLog";
            this.listBoxLog.Size = new System.Drawing.Size(698, 220);
            this.listBoxLog.TabIndex = 41;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 170);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 18);
            this.label1.TabIndex = 42;
            this.label1.Text = "UserID:";
            // 
            // textBoxUserID
            // 
            this.textBoxUserID.Location = new System.Drawing.Point(97, 165);
            this.textBoxUserID.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxUserID.MaxLength = 6;
            this.textBoxUserID.Name = "textBoxUserID";
            this.textBoxUserID.Size = new System.Drawing.Size(123, 28);
            this.textBoxUserID.TabIndex = 43;
            this.textBoxUserID.Text = "jacking75";
            this.textBoxUserID.WordWrap = false;
            // 
            // textBoxUserPW
            // 
            this.textBoxUserPW.Location = new System.Drawing.Point(97, 201);
            this.textBoxUserPW.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxUserPW.MaxLength = 6;
            this.textBoxUserPW.Name = "textBoxUserPW";
            this.textBoxUserPW.Size = new System.Drawing.Size(123, 28);
            this.textBoxUserPW.TabIndex = 45;
            this.textBoxUserPW.Text = "jacking75";
            this.textBoxUserPW.WordWrap = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(14, 208);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(81, 18);
            this.label2.TabIndex = 44;
            this.label2.Text = "PassWD:";
            // 
            // button2
            // 
            this.button2.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button2.Location = new System.Drawing.Point(14, 242);
            this.button2.Margin = new System.Windows.Forms.Padding(4);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(207, 39);
            this.button2.TabIndex = 46;
            this.button2.Text = "Login";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Room
            // 
            this.Room.Controls.Add(this.btnMatching);
            this.Room.Controls.Add(this.btnRoomChat);
            this.Room.Controls.Add(this.textBoxRoomSendMsg);
            this.Room.Controls.Add(this.listBoxRoomChatMsg);
            this.Room.Controls.Add(this.label4);
            this.Room.Controls.Add(this.listBoxRoomUserList);
            this.Room.Controls.Add(this.btn_RoomLeave);
            this.Room.Controls.Add(this.btn_RoomEnter);
            this.Room.Controls.Add(this.textBoxRoomNumber);
            this.Room.Controls.Add(this.label3);
            this.Room.Location = new System.Drawing.Point(19, 382);
            this.Room.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.Room.Name = "Room";
            this.Room.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.Room.Size = new System.Drawing.Size(707, 375);
            this.Room.TabIndex = 47;
            this.Room.TabStop = false;
            this.Room.Text = "Room";
            // 
            // btnMatching
            // 
            this.btnMatching.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnMatching.Location = new System.Drawing.Point(424, 28);
            this.btnMatching.Margin = new System.Windows.Forms.Padding(4);
            this.btnMatching.Name = "btnMatching";
            this.btnMatching.Size = new System.Drawing.Size(270, 41);
            this.btnMatching.TabIndex = 54;
            this.btnMatching.Text = "Matching";
            this.btnMatching.UseVisualStyleBackColor = true;
            this.btnMatching.Click += new System.EventHandler(this.btnMatching_Click);
            // 
            // btnRoomChat
            // 
            this.btnRoomChat.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnRoomChat.Location = new System.Drawing.Point(624, 318);
            this.btnRoomChat.Margin = new System.Windows.Forms.Padding(4);
            this.btnRoomChat.Name = "btnRoomChat";
            this.btnRoomChat.Size = new System.Drawing.Size(71, 39);
            this.btnRoomChat.TabIndex = 53;
            this.btnRoomChat.Text = "chat";
            this.btnRoomChat.UseVisualStyleBackColor = true;
            this.btnRoomChat.Click += new System.EventHandler(this.btnRoomChat_Click);
            // 
            // textBoxRoomSendMsg
            // 
            this.textBoxRoomSendMsg.Location = new System.Drawing.Point(19, 322);
            this.textBoxRoomSendMsg.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxRoomSendMsg.MaxLength = 32;
            this.textBoxRoomSendMsg.Name = "textBoxRoomSendMsg";
            this.textBoxRoomSendMsg.Size = new System.Drawing.Size(597, 28);
            this.textBoxRoomSendMsg.TabIndex = 52;
            this.textBoxRoomSendMsg.Text = "test1";
            this.textBoxRoomSendMsg.WordWrap = false;
            // 
            // listBoxRoomChatMsg
            // 
            this.listBoxRoomChatMsg.FormattingEnabled = true;
            this.listBoxRoomChatMsg.ItemHeight = 18;
            this.listBoxRoomChatMsg.Location = new System.Drawing.Point(206, 98);
            this.listBoxRoomChatMsg.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.listBoxRoomChatMsg.Name = "listBoxRoomChatMsg";
            this.listBoxRoomChatMsg.Size = new System.Drawing.Size(488, 202);
            this.listBoxRoomChatMsg.TabIndex = 51;
            this.listBoxRoomChatMsg.SelectedIndexChanged += new System.EventHandler(this.listBoxRoomChatMsg_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(14, 76);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(84, 18);
            this.label4.TabIndex = 50;
            this.label4.Text = "User List:";
            // 
            // listBoxRoomUserList
            // 
            this.listBoxRoomUserList.FormattingEnabled = true;
            this.listBoxRoomUserList.ItemHeight = 18;
            this.listBoxRoomUserList.Location = new System.Drawing.Point(19, 99);
            this.listBoxRoomUserList.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.listBoxRoomUserList.Name = "listBoxRoomUserList";
            this.listBoxRoomUserList.Size = new System.Drawing.Size(174, 202);
            this.listBoxRoomUserList.TabIndex = 49;
            // 
            // btn_RoomLeave
            // 
            this.btn_RoomLeave.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btn_RoomLeave.Location = new System.Drawing.Point(309, 28);
            this.btn_RoomLeave.Margin = new System.Windows.Forms.Padding(4);
            this.btn_RoomLeave.Name = "btn_RoomLeave";
            this.btn_RoomLeave.Size = new System.Drawing.Size(94, 39);
            this.btn_RoomLeave.TabIndex = 48;
            this.btn_RoomLeave.Text = "Leave";
            this.btn_RoomLeave.UseVisualStyleBackColor = true;
            this.btn_RoomLeave.Click += new System.EventHandler(this.btn_RoomLeave_Click);
            // 
            // btn_RoomEnter
            // 
            this.btn_RoomEnter.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btn_RoomEnter.Location = new System.Drawing.Point(206, 27);
            this.btn_RoomEnter.Margin = new System.Windows.Forms.Padding(4);
            this.btn_RoomEnter.Name = "btn_RoomEnter";
            this.btn_RoomEnter.Size = new System.Drawing.Size(94, 39);
            this.btn_RoomEnter.TabIndex = 47;
            this.btn_RoomEnter.Text = "Enter";
            this.btn_RoomEnter.UseVisualStyleBackColor = true;
            this.btn_RoomEnter.Click += new System.EventHandler(this.btn_RoomEnter_Click);
            // 
            // textBoxRoomNumber
            // 
            this.textBoxRoomNumber.Location = new System.Drawing.Point(140, 30);
            this.textBoxRoomNumber.Margin = new System.Windows.Forms.Padding(4);
            this.textBoxRoomNumber.MaxLength = 6;
            this.textBoxRoomNumber.Name = "textBoxRoomNumber";
            this.textBoxRoomNumber.Size = new System.Drawing.Size(53, 28);
            this.textBoxRoomNumber.TabIndex = 44;
            this.textBoxRoomNumber.Text = "0";
            this.textBoxRoomNumber.WordWrap = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 38);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(131, 18);
            this.label3.TabIndex = 43;
            this.label3.Text = "Room Number:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button5);
            this.groupBox1.Controls.Add(this.button4);
            this.groupBox1.Controls.Add(this.button3);
            this.groupBox1.Controls.Add(this.listBoxLobby);
            this.groupBox1.Location = new System.Drawing.Point(230, 165);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox1.Size = new System.Drawing.Size(496, 210);
            this.groupBox1.TabIndex = 48;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Lobby";
            // 
            // button5
            // 
            this.button5.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button5.Location = new System.Drawing.Point(330, 126);
            this.button5.Margin = new System.Windows.Forms.Padding(4);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(153, 39);
            this.button5.TabIndex = 53;
            this.button5.Text = "Leave";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // button4
            // 
            this.button4.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button4.Location = new System.Drawing.Point(330, 78);
            this.button4.Margin = new System.Windows.Forms.Padding(4);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(153, 39);
            this.button4.TabIndex = 52;
            this.button4.Text = "Enter";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // button3
            // 
            this.button3.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button3.Location = new System.Drawing.Point(330, 30);
            this.button3.Margin = new System.Windows.Forms.Padding(4);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(153, 39);
            this.button3.TabIndex = 51;
            this.button3.Text = "List 요청";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // listBoxLobby
            // 
            this.listBoxLobby.FormattingEnabled = true;
            this.listBoxLobby.ItemHeight = 18;
            this.listBoxLobby.Location = new System.Drawing.Point(16, 28);
            this.listBoxLobby.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.listBoxLobby.Name = "listBoxLobby";
            this.listBoxLobby.Size = new System.Drawing.Size(304, 166);
            this.listBoxLobby.TabIndex = 50;
            // 
            // xPosTextNumber
            // 
            this.xPosTextNumber.Location = new System.Drawing.Point(36, 756);
            this.xPosTextNumber.Margin = new System.Windows.Forms.Padding(4);
            this.xPosTextNumber.MaxLength = 6;
            this.xPosTextNumber.Name = "xPosTextNumber";
            this.xPosTextNumber.Size = new System.Drawing.Size(53, 28);
            this.xPosTextNumber.TabIndex = 55;
            this.xPosTextNumber.Text = "0";
            this.xPosTextNumber.WordWrap = false;
            // 
            // yPosTextNumber
            // 
            this.yPosTextNumber.Location = new System.Drawing.Point(104, 756);
            this.yPosTextNumber.Margin = new System.Windows.Forms.Padding(4);
            this.yPosTextNumber.MaxLength = 6;
            this.yPosTextNumber.Name = "yPosTextNumber";
            this.yPosTextNumber.Size = new System.Drawing.Size(53, 28);
            this.yPosTextNumber.TabIndex = 56;
            this.yPosTextNumber.Text = "0";
            this.yPosTextNumber.WordWrap = false;
            // 
            // PutStoneBtn
            // 
            this.PutStoneBtn.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.PutStoneBtn.Location = new System.Drawing.Point(185, 756);
            this.PutStoneBtn.Margin = new System.Windows.Forms.Padding(4);
            this.PutStoneBtn.Name = "PutStoneBtn";
            this.PutStoneBtn.Size = new System.Drawing.Size(134, 32);
            this.PutStoneBtn.TabIndex = 55;
            this.PutStoneBtn.Text = "PutStone";
            this.PutStoneBtn.UseVisualStyleBackColor = true;
            this.PutStoneBtn.Click += new System.EventHandler(this.btn_PutStoneClick);
            // 
            // GameStartBtn
            // 
            this.GameStartBtn.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.GameStartBtn.Location = new System.Drawing.Point(442, 752);
            this.GameStartBtn.Margin = new System.Windows.Forms.Padding(4);
            this.GameStartBtn.Name = "GameStartBtn";
            this.GameStartBtn.Size = new System.Drawing.Size(270, 41);
            this.GameStartBtn.TabIndex = 55;
            this.GameStartBtn.Text = "GameStart";
            this.GameStartBtn.UseVisualStyleBackColor = true;
            this.GameStartBtn.Click += new System.EventHandler(this.btn_GameStartClick);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(749, 1080);
            this.Controls.Add(this.GameStartBtn);
            this.Controls.Add(this.PutStoneBtn);
            this.Controls.Add(this.yPosTextNumber);
            this.Controls.Add(this.xPosTextNumber);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.Room);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.textBoxUserPW);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBoxUserID);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.labelStatus);
            this.Controls.Add(this.listBoxLog);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.textSendText);
            this.Controls.Add(this.btnDisconnect);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.groupBox5);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "mainForm";
            this.Text = "네트워크 테스트 클라이언트";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.mainForm_FormClosing);
            this.Load += new System.EventHandler(this.mainForm_Load);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.Room.ResumeLayout(false);
            this.Room.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnDisconnect;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox textBoxPort;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.CheckBox checkBoxLocalHostIP;
        private System.Windows.Forms.TextBox textBoxIP;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox textSendText;
        private System.Windows.Forms.Label labelStatus;
        private System.Windows.Forms.ListBox listBoxLog;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxUserID;
        private System.Windows.Forms.TextBox textBoxUserPW;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.GroupBox Room;
        private System.Windows.Forms.Button btn_RoomLeave;
        private System.Windows.Forms.Button btn_RoomEnter;
        private System.Windows.Forms.TextBox textBoxRoomNumber;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnRoomChat;
        private System.Windows.Forms.TextBox textBoxRoomSendMsg;
        private System.Windows.Forms.ListBox listBoxRoomChatMsg;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ListBox listBoxRoomUserList;
        private System.Windows.Forms.Button btnMatching;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.ListBox listBoxLobby;
        private System.Windows.Forms.TextBox xPosTextNumber;
        private System.Windows.Forms.TextBox yPosTextNumber;
        private System.Windows.Forms.Button PutStoneBtn;
        private System.Windows.Forms.Button GameStartBtn;
    }
}


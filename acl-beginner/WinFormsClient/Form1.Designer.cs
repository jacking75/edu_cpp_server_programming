namespace WinFormsClient;

partial class Form1
{
    private System.ComponentModel.IContainer components = null;

    protected override void Dispose(bool disposing)
    {
        if (disposing && (components != null))
        {
            components.Dispose();
        }
        base.Dispose(disposing);
    }

    private void InitializeComponent()
    {
        txtServerAddress = new TextBox();
        btnConnect = new Button();
        txtUserID = new TextBox();
        txtPassword = new TextBox();
        btnLogin = new Button();
        txtRoomNumber = new TextBox();
        btnEnterRoom = new Button();
        txtChatMessage = new TextBox();
        btnSendChat = new Button();
        lblDescription = new Label();
        lstUsers = new ListBox();
        lstChatMessages = new ListBox();
        lstLog = new ListBox();
        label1 = new Label();
        label2 = new Label();
        label3 = new Label();
        label4 = new Label();
        label5 = new Label();
        label6 = new Label();
        SuspendLayout();
        // 
        // txtServerAddress
        // 
        txtServerAddress.Location = new Point(118, 60);
        txtServerAddress.Name = "txtServerAddress";
        txtServerAddress.Size = new Size(260, 31);
        txtServerAddress.TabIndex = 0;
        txtServerAddress.Text = "127.0.0.1";
        // 
        // btnConnect
        // 
        btnConnect.Location = new Point(384, 60);
        btnConnect.Name = "btnConnect";
        btnConnect.Size = new Size(131, 31);
        btnConnect.TabIndex = 1;
        btnConnect.Text = "Connect";
        btnConnect.UseVisualStyleBackColor = true;
        btnConnect.Click += btnConnect_Click;
        // 
        // txtUserID
        // 
        txtUserID.Location = new Point(117, 148);
        txtUserID.Name = "txtUserID";
        txtUserID.Size = new Size(260, 31);
        txtUserID.TabIndex = 2;
        // 
        // txtPassword
        // 
        txtPassword.Location = new Point(117, 194);
        txtPassword.Name = "txtPassword";
        txtPassword.Size = new Size(260, 31);
        txtPassword.TabIndex = 3;
        // 
        // btnLogin
        // 
        btnLogin.Location = new Point(383, 159);
        btnLogin.Name = "btnLogin";
        btnLogin.Size = new Size(131, 57);
        btnLogin.TabIndex = 4;
        btnLogin.Text = "Login";
        btnLogin.UseVisualStyleBackColor = true;
        btnLogin.Click += btnLogin_Click;
        // 
        // txtRoomNumber
        // 
        txtRoomNumber.Location = new Point(309, 313);
        txtRoomNumber.Name = "txtRoomNumber";
        txtRoomNumber.Size = new Size(68, 31);
        txtRoomNumber.TabIndex = 5;
        txtRoomNumber.Text = "1";
        // 
        // btnEnterRoom
        // 
        btnEnterRoom.Location = new Point(383, 313);
        btnEnterRoom.Name = "btnEnterRoom";
        btnEnterRoom.Size = new Size(131, 31);
        btnEnterRoom.TabIndex = 6;
        btnEnterRoom.Text = "Enter Room";
        btnEnterRoom.UseVisualStyleBackColor = true;
        btnEnterRoom.Click += btnEnterRoom_Click;
        // 
        // txtChatMessage
        // 
        txtChatMessage.Location = new Point(117, 679);
        txtChatMessage.Name = "txtChatMessage";
        txtChatMessage.Size = new Size(260, 31);
        txtChatMessage.TabIndex = 7;
        // 
        // btnSendChat
        // 
        btnSendChat.Location = new Point(383, 679);
        btnSendChat.Name = "btnSendChat";
        btnSendChat.Size = new Size(131, 31);
        btnSendChat.TabIndex = 8;
        btnSendChat.Text = "Send Chat";
        btnSendChat.UseVisualStyleBackColor = true;
        btnSendChat.Click += btnSendChat_Click;
        // 
        // lblDescription
        // 
        lblDescription.AutoSize = true;
        lblDescription.Location = new Point(71, 10);
        lblDescription.Name = "lblDescription";
        lblDescription.Size = new Size(392, 25);
        lblDescription.TabIndex = 10;
        lblDescription.Text = "서버에 접속한 후, 로그인하고 방에 입장하세요.";
        // 
        // lstUsers
        // 
        lstUsers.ItemHeight = 25;
        lstUsers.Location = new Point(47, 391);
        lstUsers.Name = "lstUsers";
        lstUsers.Size = new Size(123, 154);
        lstUsers.TabIndex = 0;
        // 
        // lstChatMessages
        // 
        lstChatMessages.FormattingEnabled = true;
        lstChatMessages.ItemHeight = 25;
        lstChatMessages.Location = new Point(177, 363);
        lstChatMessages.Margin = new Padding(4);
        lstChatMessages.Name = "lstChatMessages";
        lstChatMessages.Size = new Size(337, 304);
        lstChatMessages.TabIndex = 0;
        // 
        // lstLog
        // 
        lstLog.ItemHeight = 25;
        lstLog.Location = new Point(561, 60);
        lstLog.Name = "lstLog";
        lstLog.Size = new Size(608, 379);
        lstLog.TabIndex = 17;
        // 
        // label1
        // 
        label1.AutoSize = true;
        label1.Location = new Point(22, 63);
        label1.Name = "label1";
        label1.Size = new Size(90, 25);
        label1.TabIndex = 11;
        label1.Text = "서버 주소";
        // 
        // label2
        // 
        label2.AutoSize = true;
        label2.Location = new Point(64, 150);
        label2.Name = "label2";
        label2.Size = new Size(46, 25);
        label2.TabIndex = 12;
        label2.Text = "ID : ";
        // 
        // label3
        // 
        label3.AutoSize = true;
        label3.Location = new Point(56, 197);
        label3.Name = "label3";
        label3.Size = new Size(55, 25);
        label3.TabIndex = 13;
        label3.Text = "PW : ";
        // 
        // label4
        // 
        label4.AutoSize = true;
        label4.Location = new Point(47, 316);
        label4.Name = "label4";
        label4.Size = new Size(256, 25);
        label4.TabIndex = 14;
        label4.Text = "방 번호를 입력 후 입장하세요.";
        // 
        // label5
        // 
        label5.AutoSize = true;
        label5.Location = new Point(47, 682);
        label5.Name = "label5";
        label5.Size = new Size(64, 25);
        label5.TabIndex = 15;
        label5.Text = "채팅 : ";
        // 
        // label6
        // 
        label6.AutoSize = true;
        label6.Location = new Point(64, 363);
        label6.Name = "label6";
        label6.Size = new Size(84, 25);
        label6.TabIndex = 16;
        label6.Text = "유저목록";
        // 
        // Form1
        // 
        ClientSize = new Size(1396, 789);
        Controls.Add(label6);
        Controls.Add(label5);
        Controls.Add(label4);
        Controls.Add(label3);
        Controls.Add(label2);
        Controls.Add(label1);
        Controls.Add(lstUsers);
        Controls.Add(lstChatMessages);
        Controls.Add(lblDescription);
        Controls.Add(btnSendChat);
        Controls.Add(txtChatMessage);
        Controls.Add(btnEnterRoom);
        Controls.Add(txtRoomNumber);
        Controls.Add(btnLogin);
        Controls.Add(txtPassword);
        Controls.Add(txtUserID);
        Controls.Add(btnConnect);
        Controls.Add(txtServerAddress);
        Controls.Add(lstLog);
        Name = "Form1";
        Text = "Test Form";
        FormClosing += Form1_FormClosing;
        ResumeLayout(false);
        PerformLayout();
    }

    private System.Windows.Forms.ListBox lstLog;
    private System.Windows.Forms.TextBox txtServerAddress;
    private System.Windows.Forms.Button btnConnect;
    private System.Windows.Forms.TextBox txtUserID;
    private System.Windows.Forms.TextBox txtPassword;
    private System.Windows.Forms.Button btnLogin;
    private System.Windows.Forms.TextBox txtRoomNumber;
    private System.Windows.Forms.Button btnEnterRoom; 
    private System.Windows.Forms.TextBox txtChatMessage; 
    private System.Windows.Forms.Button btnSendChat;
    private System.Windows.Forms.TextBox txtChatDisplay;
    private System.Windows.Forms.ListBox lstUsers;  // 유저 목록 리스트 박스
    private System.Windows.Forms.ListBox lstChatMessages;  // 채팅 메시지 리스트 박스
    private System.Windows.Forms.Label lblDescription;
    private Label label1;
    private Label label2;
    private Label label3;
    private Label label4;
    private Label label5;
    private Label label6;
}

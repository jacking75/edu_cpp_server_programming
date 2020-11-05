using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Media;


namespace csharp_test_client
{
    public partial class mainForm
    {
        enum 돌종류 { 없음, 흑돌, 백돌 };
        private int 시작위치 = 30;
        private int 눈금크기 = 30;
        private int 돌크기 = 20;
        private int 화점크기 = 10;
        private int 바둑판크기 = 19;
        private Stack<Point> st = new Stack<Point>();


        private int 전x좌표 = -1, 전y좌표 = -1;
        private int 전돌x좌표 = -1, 전돌y좌표 = -1;
        private int 현재돌x좌표 = -1, 현재돌y좌표 = -1;
        private Pen 검은펜 = new Pen(Color.Black);
        private SolidBrush 빨간색 = new SolidBrush(Color.Red);
        private SolidBrush 검은색 = new SolidBrush(Color.Black);
        private SolidBrush 흰색 = new SolidBrush(Color.White);

        private int[,] 바둑판 = new int[19, 19];
        private bool 흑돌차례 = true;
        private bool 게임종료 = false;
        private bool 삼삼 = false;
        private bool AI모드 = true;
        private 돌종류 컴퓨터돌;

        private SoundPlayer 시작효과음 = new SoundPlayer("/sound/대국시작");
        private SoundPlayer 종료효과음 = new SoundPlayer("/sound/대국종료");
        private SoundPlayer 승리효과음 = new SoundPlayer("/sound/대국승리");
        private SoundPlayer 바둑돌소리 = new SoundPlayer("/sound/바둑돌소리");
        private SoundPlayer 무르기요청 = new SoundPlayer("/sound/무르기");




    }
}

#include "print.h"
#include "figlet.h"
#include "string.h"

char figlet_big[7][4096];             //< 缓冲区
static char figlet_color[7][4096]; // 使用1字节存储颜色索引
static int color_offset = 0;          // 当前颜色偏移量
int figlet_lens = 0;

// 定义一个数组来存储所有字符的ASCII艺术表示
FigletChar figlet_chars[256] = {
    // 控制字符 (0-31) 留空
    {0, {"", "", "", "", "", "", ""}},
    // ... 其他控制字符类似处理

    // 空格 (32)
    {
        ' ',
        {"     ",
         "     ",
         "     ",
         "     ",
         "     ",
         "     ",
         "     "}},

    // 感叹号 (33)
    {
        '!',
        {"  _ ",
         " | |",
         " | |",
         " | |",
         " |_|",
         " (_)",
         "    "}},

    // 双引号 (34)
    {
        '"',
        {"  _ _ ",
         " ( | )",
         "  V V ",
         "      ",
         "      ",
         "      ",
         "      "}},

    // 井号 (35)
    {
        '#',
        {"    _  _   ",
         "  _| || |_ ",
         " |_  __  _|",
         "  _| || |_ ",
         " |_  __  _|",
         "   |_||_|  ",
         "           "}},

    // 美元符号 (36)
    {
        '$',
        {"   _  ",
         "  | | ",
         " / __)",
         " \\__ \\",
         " (   /",
         "  |_| ",
         "      "}},

    // 百分号 (37)
    {
        '%',
        {"  _   __",
         " (_) / /",
         "    / / ",
         "   / /  ",
         "  / / _ ",
         " /_/ (_)",
         "        "}},

    // 与符号 (38)
    {
        '&',
        {"         ",
         "   ___   ",
         "  ( _ )  ",
         "  / _ \\/\\",
         " | (_>  <",
         "  \\___/\\/",
         "         "}},

    // 单引号 (39)
    {
        '\'',
        {"  _ ",
         " ( )",
         " |/ ",
         "    ",
         "    ",
         "    ",
         "    "}},

    // 左括号 (40)
    {
        '(',
        {"   __",
         "  / /",
         " | | ",
         " | | ",
         " | | ",
         " | | ",
         "  \\_\\"}},

    // 右括号 (41)
    {
        ')',
        {" __  ",
         " \\ \\ ",
         "  | |",
         "  | |",
         "  | |",
         "  | |",
         " /_/ "}},

    // 星号 (42)
    {
        '*',
        {"     _    ",
         "  /\\| |/\\ ",
         "  \\ ` ' / ",
         " |_     _|",
         "  / , . \\ ",
         "  \\/|_|\\/ ",
         "          "}},

    // 加号 (43)
    {
        '+',
        {"        ",
         "    _   ",
         "  _| |_ ",
         " |_   _|",
         "   |_|  ",
         "        ",
         "        "}},

    // 逗号 (44)
    {
        ',',
        {"    ",
         "    ",
         "    ",
         "    ",
         "  _ ",
         " ( )",
         " |/ "}},

    // 减号 (45)
    {
        '-',
        {"         ",
         "         ",
         "  ______ ",
         " |______|",
         "         ",
         "         ",
         "         "}},

    // 句号 (46)
    {
        '.',
        {"    ",
         "    ",
         "    ",
         "    ",
         "  _ ",
         " (_)",
         "    "}},

    // 斜杠 (47)
    {
        '/',
        {"      __",
         "     / /",
         "    / / ",
         "   / /  ",
         "  / /   ",
         " /_/    ",
         "        "}},

    // 数字0-9
    {
        '0',
        {"   ___  ",
         "  / _ \\ ",
         " | | | |",
         " | | | |",
         " | |_| |",
         "  \\___/ ",
         "        "}},
    {'1',
     {"  _ ",
      " / |",
      " | |",
      " | |",
      " | |",
      " |_|",
      "    "}},
    {'2',
     {"  ___ ",
      " |__ \\ ",
      "    ) |",
      "   / / ",
      "  / /_ ",
      " |____|",
      "       "}},
    {'3',
     {"  ____  ",
      " |___ \\ ",
      "   __) |",
      "  |__ < ",
      "  ___) |",
      " |____/ ",
      "        "}},
    {'4',
     {"  _  _   ",
      " | || |  ",
      " | || |_ ",
      " |__   _|",
      "    | |  ",
      "    |_|  ",
      "         "}},
    {'5',
     {"  _____ ",
      " | ____|",
      " | |__  ",
      " |___ \\ ",
      "  ___) |",
      " |____/ ",
      "        "}},
    {'6',
     {
         "    __  ",
         "   / /  ",
         "  / /_  ",
         " | '_ \\ ",
         " | (_) |",
         "  \\___/ ",
         "        ",
     }},
    {'7',
     {"  ______ ",
      " |____  |",
      "     / / ",
      "    / /  ",
      "   / /   ",
      "  /_/    ",
      "         "}},
    {'8',
     {"   ___  ",
      "  / _ \\ ",
      " | (_) |",
      "  > _ < ",
      " | (_) |",
      "  \\___/ ",
      "        "}},
    {'9',
     {"   ___  ",
      "  / _ \\ ",
      " | (_) |",
      "  \\__, |",
      "    / / ",
      "   /_/  ",
      "        "}},

    // 冒号 (58)
    {
        ':',
        {"    ",
         "  _ ",
         " (_)",
         "    ",
         "  _ ",
         " (_)",
         "    "}},

    // 分号 (59)
    {
        ';',
        {"    ",
         "  _ ",
         " (_)",
         "    ",
         "  _ ",
         " ( )",
         " |/ "}},

    // 小于号 (60)
    {
        '<',
        {"    __",
         "   / /",
         "  / / ",
         " < <  ",
         "  \\ \\ ",
         "   \\_\\",
         "      "}},

    // 等于号 (61)
    {
        '=',
        {"         ",
         "         ",
         "  ______ ",
         " |______|",
         "  ______ ",
         " |______|",
         "         "}},

    // 大于号 (62)
    {
        '>',
        {" __   ",
         " \\ \\  ",
         "  \\ \\ ",
         "   > >",
         "  / / ",
         " /_/  ",
         "      "}},

    // 问号 (63)

    {
        '?',
        {
            "  ____  ",
            " | __ \\ ",
            "     ) |",
            "    / / ",
            "   | |  ",
            "   |_|  ",
            "   (_)  ",
        }},

    // @符号 (64)
    {
        '@',
        {"          ",
         "    ____  ",
         "   / __ \\ ",
         "  / / _` |",
         " | | (_| |",
         "  \\ \\__,_|",
         "   \\____/ "}},

    // 大写字母A-Z
    {
        'A',
        {
            "     _     ",
            "    / \\    ",
            "   / _ \\   ",
            "  / /_\\ \\  ",
            " / /   \\ \\ ",
            "/_/     \\_\\",
            "           ",
        }},
    {'B',
     {"  ____  ",
      " |  _ \\ ",
      " | |_) )",
      " |  _ ( ",
      " | |_) )",
      " |____/ ",
      "        "}},
    {'C',
     {"   _____ ",
      "  / ____|",
      " | |     ",
      " | |     ",
      " | |____ ",
      "  \\_____|",
      "         "}},
    {'D',
     {"  _____  ",
      " |  __ \\ ",
      " | |  | |",
      " | |  | |",
      " | |__| |",
      " |_____/ ",
      "         "}},
    {'E',
     {"  _____ ",
      " |  ___)",
      " | |_   ",
      " |  _)  ",
      " | |___ ",
      " |_____)",
      "        "}},
    {'F',
     {"  _____ ",
      " |  ___)",
      " | |_   ",
      " |  _)  ",
      " | |    ",
      " |_|    ",
      "        "}},
    {'G',
     {"   _____ ",
      "  / ____|",
      " | |     ",
      " | | ___ ",
      " | |__| |",
      "  \\____/ ",
      "         "}},
    {'H',
     {"  _    _ ",
      " | |  | |",
      " | |__| |",
      " |  __  |",
      " | |  | |",
      " |_|  |_|",
      "         "}},
    {'I',
     {"  ___ ",
      " (   )",
      "  | | ",
      "  | | ",
      "  | | ",
      " (___)",
      "      "}},
    {'J',
     {"       _ ",
      "      | |",
      "      | |",
      "      | |",
      "  _   | |",
      " | |__| |",
      "  \\____/ "}},
    {'K',
     {"  _   __",
      " | | / /",
      " | |/ / ",
      " |   <  ",
      " | |\\ \\ ",
      " |_| \\_\\",
      "        "}},
    {'L',
     {"  _      ",
      " | |     ",
      " | |     ",
      " | |     ",
      " | |____ ",
      " |______|",
      "         "}},
    {'M',
     {"  __     __ ",
      " |  \\   /  |",
      " | | \\ / | |",
      " | |  V  | |",
      " | |     | |",
      " |_|     |_|",
      "            "}},
    {'N',
     {"  _     _ ",
      " | \\   | |",
      " | \\\\  | |",
      " | |\\\\ | |",
      " | | \\\\| |",
      " |_|  \\__|",
      "          "}},
    {'O',
     {"   ___  ",
      "  / _ \\ ",
      " | | | |", // 更对称的中间部分
      " | | | |", // 调整下划线位置
      " | |_| |",
      "  \\___/ ", // 底部闭合
      "        "}},
    {'P',
     {"  ____  ",
      " |  _ \\ ",
      " | |_) |",
      " |  __/ ",
      " | |    ",
      " |_|    ",
      "        "}},
    {'Q',
     {"   ____  ",
      "  / __ \\ ",
      " | |  | |",
      " | |  | |",
      " | |__| |",
      "  \\___\\_\\",
      "         "}},
    {'R',
     {"  _____  ",
      " |  __ \\ ",
      " | |__) |",
      " |  _  / ",
      " | | \\ \\ ",
      " |_|  \\_\\",
      "         "}},
    {'S',
     {"   _____ ",
      "  / ____|",
      " | (___  ",
      "  \\___ \\ ",
      "  ____) |",
      " |_____/ ",
      "         "}},
    {'T',
     {"  _______ ",
      " |__   __|",
      "    | |   ",
      "    | |   ",
      "    | |   ",
      "    |_|   ",
      "          "}},
    {'U',
     {"  _    _ ",
      " | |  | |",
      " | |  | |",
      " | |  | |",
      " | |__| |",
      "  \\____/ ",
      "         "}},
    {'V',
     {" __      __",
      " \\ \\    / /",
      "  \\ \\  / / ",
      "   \\ \\/ /  ",
      "    \\  /   ",
      "     \\/    ",
      "           "}},
    {'W',
     {" __          __",
      " \\ \\        / /",
      "  \\ \\  /\\  / / ",
      "   \\ \\/  \\/ /  ",
      "    \\  /\\  /   ",
      "     \\/  \\/    ",
      "               "}},
    {'X',
     {" __   __",
      " \\ \\ / /",
      "  \\ V / ",
      "   > <  ",
      "  / . \\ ",
      " /_/ \\_\\",
      "        "}},
    {'Y',
     {" __     __",
      " \\ \\   / /",
      "  \\ \\_/ / ",
      "   \\   /  ",
      "    | |   ",
      "    |_|   ",
      "          "}},
    {'Z',
     {"  ______",
      " |___  /",
      "    / / ",
      "   / /  ",
      "  / /__ ",
      " /_____|",
      "        "}},

    // 左方括号 (91)
    {
        '[',
        {"  ___ ",
         " |  _|",
         " | |  ",
         " | |  ",
         " | |  ",
         " | |_ ",
         " |___|"}},

    // 反斜杠 (92)
    {
        '\\',
        {" __     ",
         " \\ \\    ",
         "  \\ \\   ",
         "   \\ \\  ",
         "    \\ \\ ",
         "     \\_\\",
         "        "}},

    // 右方括号 (93)
    {
        ']',
        {"  ___ ",
         " |_  |",
         "   | |",
         "   | |",
         "   | |",
         "  _| |",
         " |___|"}},

    // 脱字符 (94)
    {
        '^',
        {"  /\\ ",
         " |/\\|",
         "     ",
         "     ",
         "     ",
         "     ",
         "     "}},

    // 下划线 (95)
    {
        '_',
        {"         ",
         "         ",
         "         ",
         "         ",
         "         ",
         "  ______ ",
         " |______|"}},

    // 反引号 (96)
    {
        '`',
        {"  _ ",
         " ( )",
         "  \\|",
         "    ",
         "    ",
         "    ",
         "    "}},

    // 小写字母a-z
    {
        'a',
        {"        ",
         "        ",
         "   __ _ ",
         "  / _` |",
         " | (_| |",
         "  \\__,_|",
         "        "}},
    {'b',
     {"        ",
      "  _     ",
      " | |__  ",
      " | '_ \\ ",
      " | |_) |",
      " |_.__/ ",
      "        "}},
    {'c',
     {"       ",
      "       ",
      "   ___ ",
      "  / __|",
      " | (__ ",
      "  \\___|",
      "       "}},
    {'d',
     {"       _ ",
      "      | |",
      "   ___| |",
      "  /  _' |",
      " |  (_| |",
      "  \\___._|",
      "         "}},
    {'e',
     {"       ",
      "       ",
      "   ___ ",
      "  / _ \\",
      " |  __/",
      "  \\___|",
      "       "}},
    {'f',
     {"   __ ",
      "  / _|",
      " | |_ ",
      " |  _|",
      " | |  ",
      " |_|  ",
      "      "}},
    {'g', //< 多了一行
     {
         //  "        ",
         "        ",
         "   ____ ",
         "  / _  |",
         " | (_| |",
         "  \\__  |",
         "   __/ |",
         "  |___/ "}},
    {'h',
     {"  _     ",
      " | |    ",
      " | |__  ",
      " | '_ \\ ",
      " | | | |",
      " |_| |_|",
      "        "}},
    {'i',
     {"  _ ",
      " (_)",
      "  _ ",
      " | |",
      " | |",
      " |_|",
      "    "}},
    {'j',
     {//          "     _",
      "    (_)",
      "     _ ",
      "    | |",
      "    | |",
      "    | |",
      "  _/ / ",
      " |__/  "}},
    {'k',
     {"  _    ",
      " | | _ ",
      " | |/ /",
      " |   < ",
      " |_|\\_\\",
      "       ",
      "       "}},
    {'l',
     {"  _ ",
      " | |",
      " | |",
      " | |",
      " | |",
      " |_|",
      "    "}},
    {'m',
     {"            ",
      "            ",
      "  _ __ ___  ",
      " | '_ ` _ \\ ",
      " | | | | | |",
      " |_| |_| |_|",
      "            "}},
    {'n',
     {"        ",
      "        ",
      "  _ __  ",
      " | '_ \\ ",
      " | | | |",
      " |_| |_|",
      "        "}},
    {'o',
     {"        ",
      "        ",
      "   ___  ",
      "  / _ \\ ",
      " | (_) |",
      "  \\___/ ",
      "        "}},
    {'p',
     {//  "        ",
      "        ",
      "  _ __  ",
      " | '_ \\ ",
      " | |_) |",
      " | .__/ ",
      " | |    ",
      " |_|    "}},
    {'q',
     {//  "         ",
      "         ",
      "   __ _  ",
      "  / _` | ",
      " | (_| | ",
      "  \\__, | ",
      "     | | ",
      "     |_| "}},
    {'r',
     {"       ",
      "       ",
      "  _ __ ",
      " | '__|",
      " | |   ",
      " |_|   ",
      "       "}},
    {'s',
     {"       ",
      "       ",
      "   ___ ",
      "  / __|",
      "  \\__ \\",
      "  |___/",
      "       "}},
    {'t',
     {"    _   ",
      "   | |  ",
      "  _| |_ ",
      " |__ __|",
      "   | |_ ",
      "    \\__|",
      "        "}},
    {'u',
     {"        ",
      "        ",
      "  _   _ ",
      " | | | |",
      " | |_| |",
      "  \\__,_|",
      "        "}},
    {'v',
     {"        ",
      "        ",
      " __   __",
      " \\ \\ / /",
      "  \\ V / ",
      "   \\_/  ",
      "        "}},
    {'w',
     {"           ",
      "           ",
      " __      __",
      " \\ \\ /\\ / /",
      "  \\ V  V / ",
      "   \\_/\\_/  ",
      "           "}},
    {'x',
     {"       ",
      "       ",
      " __  __",
      " \\ \\/ /",
      "  >  < ",
      " /_/\\_\\",
      "       "}},
    {'y',
     {//  "        ",
      "        ",
      "  _   _ ",
      " | | | |",
      " | |_| |",
      "  \\__, |",
      "   __/ |",
      "  |___/ "}},
    {'z',
     {"       ",
      "       ",
      " _____ ",
      "|__  / ",
      "  / /  ",
      " /___| ",
      "       "}},

    // 左大括号 (123)
    {
        '{',
        {"    __",
         "   / /",
         "  | | ",
         " / /  ",
         "  \\ \\ ",
         "   \\_\\",
         "      "}},

    // 竖线 (124)
    {
        '|',
        {"  _ ",
         " | |",
         " | |",
         " | |",
         " | |",
         " | |",
         " |_|"}},

    // 右大括号 (125)
    {
        '}',
        {" __   ",
         " \\ \\  ",
         "  | | ",
         "  \\ \\ ",
         " / /  ",
         "/_/   ",
         "      "}},

    // 波浪号 (126)
    {
        '~',
        {"  /\\/|",
         " |/\\/ ",
         "      ",
         "      ",
         "      ",
         "      ",
         "      "}},

};

int add_figlet_char(char c)
{
    int i;
    for (i = 0; i < 256; i++) // 查找
    {
        if (c == figlet_chars[i].code)
            break;
    }
    if (i == 256) // 没有查找到字体
    {
        printf("no font for char: %d\n", c); //<没有%c
        return 1;                            //<错误
    }
    for (int j = 0; j < 7; j++)
    {
        char *str_tocpy = figlet_chars[i].lines[j];
        // printf("%s\n",str_tocpy);
        memmove(figlet_big[j] + figlet_lens, str_tocpy, strlen(str_tocpy));
    }
    figlet_lens += strlen(figlet_chars[i].lines[0]); // 第一行的长度，要求字体每行长度相同。下一个字体才会在缓存的正确位置
    return 0;
}
int color_index = 0;
#define COLOR_NUM 6 //< 使用的颜色数量
// char *color_style[COLOR_NUM] = {
//     RED_COLOR_PRINT,
//     GREEN_COLOR_PRINT,
//     YELLOW_COLOR_PRINT,
//     BLUE_COLOR_PRINT,
//     MAGANTA_COLOR_PRINT,
//     CYAN_COLOR_PRINT,
// };

#define COLOR1 "\033[32;1m"
#define COLOR2 "\033[36;1m" 
#define COLOR3 "\033[31;1m"
#define RED_COLOR_PRINT "\033[31;1m"
#define GREEN_COLOR_PRINT "\033[32;1m"
#define YELLOW_COLOR_PRINT "\033[33;1m"
#define BLUE_COLOR_PRINT "\033[34;1m"
#define MAGANTA_COLOR_PRINT "\033[35;1m"
#define CYAN_COLOR_PRINT "\033[36;1m"
char *color_style[COLOR_NUM] = {
    COLOR1,
    COLOR1,
    COLOR2,
    COLOR2,
    COLOR3,
    COLOR3,
};
void print_figlet_buffer()
{
    char current_color[7] = {0}; // 每行独立记录颜色状态

    for (int i = 0; i < 7; i++)
    {
        current_color[i] = 0xFF; // 初始化为非法值强制首次更新

        for (int j = 0; j < figlet_lens; j++)
        {
            // 获取当前像素颜色索引
            char color_idx = figlet_color[i][j];

            // 仅当颜色变化时更新
            if (color_idx != current_color[i])
            {
                if (color_idx < COLOR_NUM)
                {
                    printf("%s", color_style[color_idx]);
                }
                else
                {
                    printf("\033[0m"); // 默认颜色
                }
                current_color[i] = color_idx;
            }

            // 打印实际字符
            consputc(figlet_big[i][j]);
        }
        printf("\033[0m\n"); // 行尾重置
    }
}

void printf_figlet(char *fmt)
{
    /*开头不清零，如果printf_figlet之前，缓冲区就有数据比如HH，输出会是HH+fmt*/
    figlet_lens = 0;

    while (*fmt) //< 把结尾的\0之前的内容的字体存入figlet_buff
    {
        if (!add_figlet_char(*fmt++))
        {
            //< add_figlet_char返回0,正确。继续执行就可以
        }
        else //< add_figlet_char返回-1,出错了
        {
            printf("未识别的字符,printf_figlet failed\n");
            return;
        }
    }
    print_figlet_buffer();

    /*结束时把figlet_lens置零，标识figlet_big可用*/
    figlet_lens = 0; //< 不需要清理figlet_big，下次打印也应该正常
}








void color_index_plus() //< color_index增加1,如果增加前已经指向最后一个就归零
{
    color_index++;
    if (color_index == COLOR_NUM)
        color_index = 0;
}
int dynamic_idx,diagonal_factor;
/*自动添加颜色*/
int add_figlet_char_auto_with_color(char c) //< color必须是"\033[31;1m"这样的控制颜色序列
{
    int i;
    for (i = 0; i < 256; i++) // 查找
    {
        if (c == figlet_chars[i].code)
            break;
    }
    if (i == 256) // 没有查找到字体
    {
        printf("no font for char: %d\n", c); //<没有%c
        return 1;                            //<错误
    }

    const int char_width = strlen(figlet_chars[i].lines[0]);
    const int color_idx = color_index % COLOR_NUM;

    // 修改后的颜色处理部分
    for (int j = 0; j < 7; j++)
    {
        /* 为每个像素设置颜色 */
        for (int k = 0; k < char_width; k++)
        {
            //dynamic_idx = (color_idx + j + color_offset) % COLOR_NUM; ///< 修改这里修改颜色算法
            diagonal_factor = j + (figlet_lens + k);
            dynamic_idx = (color_idx + diagonal_factor + color_offset) % COLOR_NUM;
            figlet_color[j][figlet_lens + k] = dynamic_idx;
        }
    }
    color_offset += 1; // 每字符颜色偏移

    // 原始字符数据拷贝保持不变
    for (int j = 0; j < 7; j++)
    {
        memmove(figlet_big[j] + figlet_lens,
                figlet_chars[i].lines[j],
                char_width);
    }
    figlet_lens += char_width; // 第一行的长度，要求字体每行长度相同。下一个字体才会在缓存的正确位置
    color_index_plus();
    return 0;
}

void printf_figlet_color(char *fmt)
{
    /*开头不清零，如果printf_figlet之前，缓冲区就有数据比如HH，输出会是HH+fmt*/
    figlet_lens = 0;

    while (*fmt) //< 把结尾的\0之前的内容的字体存入figlet_buff
    {
        if (!add_figlet_char_auto_with_color(*fmt++))
        {
            //< add_figlet_char返回0,正确。继续执行就可以
        }
        else //< add_figlet_char返回-1,出错了
        {
            printf("未识别的字符,printf_figlet failed\n");
            return;
        }
    }
    print_figlet_buffer();

    /*结束时把figlet_lens置零，标识figlet_big可用*/
    figlet_lens = 0; //< 不需要清理figlet_big，下次打印也应该正常
}

void test_figlet()
{
    // add_figlet_char('H');
    // add_figlet_char('H');
    // print_figlet_buffer();
    // printf("开始测试\n");
    // printf_figlet("SH");
    // add_figlet_char('H');
    // add_figlet_char('H');
    // printf_figlet("SS");
    // //printf_figlet("AC");
    // printf_figlet("SC7");

    /*手动添加颜色字符测试*/
    // add_figlet_char_with_color('S',RED_COLOR_PRINT);
    // add_figlet_char_with_color('C',GREEN_COLOR_PRINT);
    // print_figlet_buffer();

    /*自动添加颜色字符测试*/
    add_figlet_char_auto_with_color('S');
    add_figlet_char_auto_with_color('C');
    add_figlet_char_auto_with_color('7');
    print_figlet_buffer();

    while (1)
        ;
}

void test_figlet_all_cha()
{
    char c[2] = {' ', '\0'};
    for (int i = 32; i < 127; i++)
    {
        c[0] = i;
        printf_figlet(c);
    }
}
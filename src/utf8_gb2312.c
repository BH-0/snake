#include "utf8_gb2312.h"

//查表转换
extern unsigned short const giGB2312[21327][2];

/*!
 * \brief   计算当前字符包含字节数
 *
 * \details 通过第一个字节的高位1的数量计算
 *
 * \param   char firstCh  第一个字节
 *
 * \retval  字节数
 */
int GetUtf8ByteNumForWord( char firstCh )
{
    char temp = 0x80;
    int num = 0;

    while (temp & firstCh)
    {
        num++;
        temp = (temp >> 1);
    }
    return num;
}

const unsigned short giGBCount=21327;

/*!
 * \brief   Unicode -> Gb2312
 *
 * \details 通过查表将Unicode转为Gb2312
 *
 * \param   unicode  输入的Unicode
 *
 * \retval  转换得到的Gb2312
 */
static unsigned short getgb(unsigned short int unicode)
{
    int i;

    for(i=0; i<giGBCount; i++)
    {
        if (giGB2312[i][1] == ntohs(unicode))
        {
            return ntohs(giGB2312[i][0]);
        }
    }
    return 0xFFFF;
}

/*!
 * \brief   UTF8至Gb2312 转换函数
 *
 * \details UTF8 -> Unicode -> Gb2312
 *
 * \param   const char* gb  输入UTF8字符串
 *      int len     输入长度
 *     char *temp    输出Gb2312字符串
 *
 */
void Utf8ToGb2312(const char* utf8, int len, unsigned char *temp)
{
    int byteCount = 0;
    int i = 0;
    int j = 0;
    unsigned short int unicodeKey;

    while (i < len)
    {
        switch(GetUtf8ByteNumForWord((char)utf8[i]))
        {
            case 0:
                temp[j] = utf8[i];
                byteCount = 1;
                break;

            case 2:
                temp[j] = utf8[i];
                temp[j + 1] = utf8[i + 1];
                byteCount = 2;
                break;
            case 3:
                //这里就开始进行UTF8->Unicode
                temp[j] = ((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F);
                temp[j+1] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);
                //取得Unicode的值
                memcpy(&unicodeKey, (temp + j), 2);
                //根据这个值查表取得对应的GB2312的值
                unsigned short gbKey = getgb(unicodeKey);
                /* 因为gbKey是unsigned short类型，STM32是小端存储，保存成char类型时需要转一下 */
                if (gbKey != 0)
                {
                    gbKey = (gbKey >> 8) | (gbKey << 8);
                    memcpy((temp + j), &gbKey, 2);
                }
                byteCount = 3;
                break;
            case 4:
                byteCount = 4;
                break;
            case 5:
                byteCount = 5;
                break;
            case 6:
                byteCount = 6;
                break;
            default:
                break;
        }
        i += byteCount;
        if (byteCount == 1)
        {
            j++;
        }
        else
        {
            j += 2;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

#if 0

/*!
 * \brief   Gb2312 -> Unicode
 *
 * \details 通过查表将Gb2312转为Unicode
 *
 * \param   unicode  输入的Gb2312
 *
 * \retval  转换得到的Unicode
 */
static unsigned short getun(unsigned short int gb)
{
    int i;

    for(i=0;i<giGBCount;i++)
    {
        if (giGB2312[i][0]==gb)
        {
            return htons(giGB2312[i][1]);
        }
    }

    return 0xFFFF;
}

/*!
 * \brief   Gb2312至UTF8 转换函数
 *
 * \details Gb2312 -> Unicode -> UTF8
 *
 * \param   const char* gb  输入Gb2312字符串
 *      int len     输入长度
 *     char *temp    输出UTF8字符串
 *
 * \retval  转换得到的UTF8长度
 */
int Gb2312ToUtf8(const char* gb, int len, char *temp)
{
    int i=0,j=0;
    unsigned short iTmp;

    while(i<len)
    {
        if ((unsigned char)gb[i] <= 0xa0)
        {
            iTmp=gb[i];
            iTmp=htons(iTmp);
            i++;
        }
        else
        {
            iTmp=getun(*(unsigned short int*)(gb+i));
            i+=2;
        }
        /* 从getun返回的时候已经被转成大端了，下面再转成小端*/
        iTmp = iTmp >> 8 | iTmp << 8;//这里本没有转的必要，getun由于系统是小端将数据转了一下，这里转回来
        if(iTmp <= 0x7F)
        {
            temp[j] = iTmp;
            j++;
        }
        else if(iTmp <= 0x7FF)
        {
            temp[j] = (iTmp >> 6) | 0xC0;
            temp[j+1] = (iTmp & 0x3F) | 0x80;
            j += 2;
        }
        else if(iTmp <= 0xFFFF)
        {
            temp[j] = (iTmp >> 12) | 0xE0;
            temp[j+1] = ((iTmp >> 6) & 0x3F) | 0x80;
            temp[j+2] = (iTmp & 0x3F) | 0x80;
            j += 3;
        }
        else
        {
            /*暂不考虑支持*/
        }
    }

    return j;
}

#endif


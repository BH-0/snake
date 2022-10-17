//
// Created by 10031 on 2022/10/5.
//

#ifndef __UTF8_GB2312_H
#define __UTF8_GB2312_H
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

/*!
 * \brief   UTF8至Gb2312 转换函数
 *
 * \details UTF8 -> Unicode -> Gb2312
 *
 * \param   const char* gb  输入UTF8字符串
 *      int len     输入长度
 *     char *temp    输出Gb2312字符串
 */
void Utf8ToGb2312(const char* utf8, int len, unsigned char *temp);

/*!
 * \brief   计算当前字符包含字节数
 *
 * \details 通过第一个字节的高位1的数量计算
 *
 * \param   char firstCh  第一个字节
 *
 * \retval  字节数
 */
int GetUtf8ByteNumForWord( char firstCh );

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
int Gb2312ToUtf8(const char* gb, int len, char *temp);


#endif //__UTF8_GB2312_H

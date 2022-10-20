#include "jpeg.h"
//----------------------------------------------------------------
//此为jpeg图片相关程序
//----------------------------------------------------------------

static char g_color_buf[FB_SIZE]={0};

//获取文件大小
unsigned long file_size_get(const char *pfile_path)
{
    unsigned long filesize = -1;
    struct stat statbuff;

    if(stat(pfile_path, &statbuff) < 0)
    {
        return filesize;
    }
    else
    {
        filesize = statbuff.st_size;
    }

    return filesize;
}

//打开一张图片，入口参数路径，返回值指向图片管理结构体
bmp_t *open_jpeg(char *pjpg_path)
{
    //创建图片管理结构体
    bmp_t *jpeg = malloc(sizeof(bmp_t));
    jpeg->path = pjpg_path;

    /*?¨ò??a?????ó￡?′í?ó′|àí???ó*/
    struct 	jpeg_decompress_struct 	cinfo;
    struct 	jpeg_error_mgr 			jerr;

    char *pcolor_buf = g_color_buf;

    unsigned int 	i=0;
    int	jpg_fd;
    unsigned int 	jpg_size;

        /*打开图片文件*/
        jpg_fd = open(pjpg_path,O_RDWR);

        if(jpg_fd == -1)
        {
            printf("open %s error\n",pjpg_path);
            free(jpeg);
            return NULL;
        }

        /* ??è?jpg???tμ?′óD? */
        jpg_size=file_size_get(pjpg_path);

        unsigned char pjpg[jpg_size];
        //pjpg = malloc(jpg_size);  //不知道为啥，用堆空间释放时会提示映射释放错误，指针无效

        read(jpg_fd,pjpg,jpg_size);


    /*×￠2á3?′í′|àí*/
    cinfo.err = jpeg_std_error(&jerr);

    /*′′?¨?a??*/
    jpeg_create_decompress(&cinfo);

    /*?±?ó?a???ú′?êy?Y*/
    jpeg_mem_src(&cinfo,pjpg,jpg_size);

    /*?á???tí·*/
    jpeg_read_header(&cinfo, TRUE);

    /*?aê??a??*/
    jpeg_start_decompress(&cinfo);

////////////////////////////////////////////////////////////////////////////////
    jpeg->width = cinfo.output_width;
    jpeg->height = cinfo.output_height;
    printf("%s: height=%d, width=%d\n",jpeg->path,jpeg->height,jpeg->width);
    if((jpeg->height*jpeg->width)%4 != 0) //像素点不为4的倍数
    {
        printf("%s:Error, the image pixels in multiples of four!\n",pjpg_path);
        free(jpeg);
        //jpeg_finish_decompress(&cinfo); //销毁图片
        jpeg_destroy_decompress(&cinfo);
        close(jpg_fd);
        return NULL;
    }
    if((jpeg->height*jpeg->width)>2560*2560) //图片超大
    {
        printf("%s:Error, the image size is too big!\n",pjpg_path);
        free(jpeg);
        //jpeg_finish_decompress(&cinfo); //销毁图片
        jpeg_destroy_decompress(&cinfo);
        close(jpg_fd);
        return NULL;
    }

    jpeg->bmp_buf = malloc(jpeg->width*jpeg->height*3);

    while(cinfo.output_scanline < cinfo.output_height )
    {
        pcolor_buf = g_color_buf;

        /* ?áè?jpgò?DDμ?rgb?μ */
        jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
        for(i=0; i<cinfo.output_width; i++)
        {
            *(jpeg->bmp_buf+(cinfo.output_scanline*cinfo.output_width*3+i*3+2)) = *(pcolor_buf+i*3);
            *(jpeg->bmp_buf+(cinfo.output_scanline*cinfo.output_width*3+i*3+1)) = *(pcolor_buf+i*3+1);
            *(jpeg->bmp_buf+(cinfo.output_scanline*cinfo.output_width*3+i*3)) = *(pcolor_buf+i*3+2);
        }
    }
/////////////////////////////////////////////////////////////////////////////////

    /*?a??íê3é*/
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    if(pjpg_path!=NULL)
    {
        /* 1?±?jpg???t */
        close(jpg_fd);
        /* êí·?jpg???t?ú′????? */
        //free(pjpg);
    }

    return jpeg;
}
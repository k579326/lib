#ifndef _FILE_COMMON_H_
#define _FILE_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cp-error.h"
#include "cp-define.h"

#ifndef IN 
#define IN
#endif

#ifndef OUT 
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif


/*

NOTICE:    本库接口中所有路径的编码统一为utf8编码，切记
*/




#ifdef __cplusplus
extern "C" {
#endif
/**************************************
*@Desc:	  open file by the path
*@Param:  [in]  path      file path
*@Param:  [in]  mode      open mode,  see remark
*@Param:  [out] fd        the pointer of file description pointer ------ FILE**
*@Return: int             error code
*@Remark: example: 
                cp_file_open("xxx.txt", FO_READ | FO_BIN, fd);
            means open file by read only and text mode.
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_open(IN const char *path, IN file_op_em mode, OUT cp_fd *fd);


/**************************************
*@Desc:	  close file
*@Param:  [in] fd        
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_close(IN cp_fd fd);

/**************************************
*@Desc:	  read file content
*@Param:  [in] fd   
*@Param:  [in] buff     
*@Param:  [in] size         number of bytes you want to read. 
*@Param:  [out]readsize     the pointer of bytes number read actually, this parameter can't be NULL.
*@Return: int
*@Remark: this function don't check the length of buff, you should give buff large enough. you can get the size of target file before using this func.
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_read(IN cp_fd fd, INOUT uint8_t *buff, IN int size, OUT int *readsize);


/**************************************
*@Desc:	  write data to file 
*@Param:  [in] fd
*@Param:  [in] buff
*@Param:  [in] size         number of bytes you want to write, can't be zero or less than zero.
*@Param:  [out]readsize     the pointer of bytes number write actually, this parameter can be NULL, if you don't want to know how much bytes been writen to file.
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_write(IN cp_fd fd, IN uint8_t *buff, IN int size, OUT int *readsize);


/**************************************
*@Desc:	  move the file pointer
*@Param:  [in] fd
*@Param:  [in] offset
*@Param:  [in] pos       
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_seek(IN cp_fd fd, IN long offset, IN file_pos_em pos);


/**************************************
*@Desc:	  this function should be used to get the size of file only. 
*@Param:  [in] fd
*@Param:  [in] size
*@Return: [in] int
*@Remark:  
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_size(IN cp_fd fd, OUT int *size);


/**************************************
*@Desc:	  rename the file name
*@Param:  [in] oldname
*@Param:  [in] newname
*@Return: int
*@Remark: if function success, the old file will be deleted.
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_rename(IN const char *oldname, IN const char *newname);


/**************************************
*@Desc:	  remove the file
*@Param:  [in] path
*@Return: int
*@Remark: only remove the file, can't remove the directory
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_remove(IN const char *path);


/**************************************
*@Desc:	  copy the file to the destination path
*@Param:  [in] srcpath  
*@Param:  [in] despath
*@Return: int
*@Remark: old file will not be delete
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_copy(IN const char *srcpath, IN const char *despath, IN bool no_overwrite);


/************************************** todo
*@Desc:	  determines whether the file have read the EOF
*@Param:  [in]  fd
*@Param:  [out] flag     0： not reach the end of file   !0： have reached the end of file 
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_iseof(IN cp_fd fd, OUT bool *flag);


/**************************************
*@Desc:	  fflush
*@Param:  fd
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/02
**************************************/
int cp_file_flush(IN cp_fd fd);


/***************************************************path function*********************************************************/

/**************************************
*@Desc:	  determines whether the file exist
*@Param:  [in]  path
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/04
**************************************/
int cp_path_exist(IN const char *path);

/**************************************
*@Desc:	  get previous dir of the absolute path
*@Param:  [in] path
*@Param:  [out]predir
*@Return: int
*@Remark: the path will be changed in function.
*@Author: guok
*@Date:   2017/05/11
**************************************/
int cp_path_predir(IN const char *path, OUT char *predir);


/**************************************
*@Desc:	  get the last name from path
*@Param:  [in] path
*@Param:  [out] lastname
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/11
**************************************/
int cp_path_lastname(IN const char *path, OUT char *lastname);


/**************************************
*@Desc:	  get the full path of module (for dll)
*@Param:  function.  any one function name belong to the target shared lib
*@Param:  path.      buff recive the path, must ensure the length enough.
*@Return: int.
*@Remark: 1. can used for shared library only. if want to get executable path, use cp_path_getexepath function
          2. don't use this function on windows, instead of using cp_path_getexepath
*@Author: guok
*@Date:   2017/05/11
**************************************/
int cp_path_getlibpath(IN const char *function, OUT char *path);

/**************************************
*@Desc:	  get the full path of current process
*@Param:  path.     buff which receive the path of process, must ensure the length of buff enough.
*@Return: int.
*@Remark: 
*@Author: guok
*@Date:   2017/05/10
**************************************/
int cp_path_getexepath(OUT char* path, IN uint32_t len);


/**************************************
*@Desc:	  convert the path to suitable style. for example, on linux, this path "\usr\local\" can be converted to "/usr/local/"
*@Param:  path
*@Return: int
*@Remark:
*@Author: guok
*@Date:   2017/05/15
**************************************/
int cp_path_convert(INOUT char* path);

/**************************************
*@Desc:	  create dir (support muti-level dir)
*@Param:  path
*@Return: int
*@Remark: create dir recursively.
*@Author: guok
*@Date:   2017/05/09
**************************************/
int cp_dir_create(IN const char* path);

/**************************************
*@Desc:	  get the path of special directory.
*@Param:  path
*@Param:  folder_type
*@Param:  fcreate
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/15
**************************************/
int cp_dir_specialfolder(OUT char* path, IN file_sf_em folder_type, IN bool fcreate);


/**************************************
*@Desc:	  used as FindFirstFile 
*@Param:  path
*@Param:  handle
*@Param:  di
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/19
**************************************/
int cp_dir_open(const char* path, dir_handle* handle, file_info_t* di);


/**************************************
*@Desc:	  used as FindNextFile
*@Param:  handle
*@Param:  di
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/19
**************************************/
int cp_dir_read(dir_handle* handle, file_info_t* di);


/**************************************
*@Desc:	  close handle
*@Param:  handle
*@Return: int
*@Remark: 
*@Author: guok
*@Date:   2017/05/19
**************************************/
int cp_dir_close(dir_handle handle);


#ifdef __cplusplus
}
#endif


#endif

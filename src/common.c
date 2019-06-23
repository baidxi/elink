#include "common.h"

//用于执行shell程序，并返回结果，popen直接获取输出有时候会存在问题
/*
    Like system() and popen(), but capture both the exit code and stdout.
    @param: file, argv: are for execvp.
    @param: code: exit code of child process. If the process is terminated
                  by signals, code is negative, otherwise, it >= 0.
    @param: output: stdout of child process.
    @param: size: size of output (including the final \0).
    @return:
            - -1: fork child process failed, when code and output are undefined.
            - -2: read from child process failed, when code and output are
   undefined.
            - -3: pipe create failed, when code and output are undefined.
            -  0: OK
*/
int32_t mk_popen(const int8_t *path, int8_t *const argv[], int32_t *code, int8_t *output,
             const size_t size)
{
    pid_t pid;
    int32_t link[2];
    int32_t ret = 0;

    if (pipe(link) == -1)
    {
        return -3;
    }

    pid = fork();

    if (pid == 0)
    {
        // child process
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execvp(path, argv);
        exit(errno);
    }
    else if (pid > 0)
    {
        close(link[1]);
        bzero(output, size);
        int32_t nbytes = 0;
        int32_t total = 0;

        while (total < size - 1)
        {
            while ((nbytes = read(link[0], (output + total),
                                  (size - 1 - total))) < 0 &&
                    errno == EINTR)
                ;

            if (nbytes < 0)
            {
                log_e("read child stdout errno: %d", errno);

                ret = -2;
                break;
            }

            if (nbytes == 0)
            {
                break;
            }

            total += nbytes;
        }

        close(link[0]);
        int32_t status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            log_e("Child exited with code %d\n", WEXITSTATUS(status));
            *code = WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status))
        {
            log_e("Child terminated abnormally, signal %d\n", WTERMSIG(status));
            *code = -WTERMSIG(status);
        }
    }
    else
    {
        close(link[0]);
        close(link[1]);
        ret = -1;
    }

    return ret;
}

//数据头部检测  头部信息不加密
//数据的头部为8个字节，分为两个部分，flag和len
//数据头部低4位是flag,采用大端模式，值为0x3f721fb5
//数据头部高4位是len, 采用大端模式
//返回加密数据的长度

//TODO:本地header的检查包括4个字节 只有长度
int32_t check_msg_header(const int8_t *buf)
{
    uint32_t data_len = 0;
    if (buf == NULL)
    {
        return -1;
    }

    memcpy(&data_len, buf + ELINK_MAGIC_LEN, ELINK_CONTENT_LEN);

    return htonl(data_len);
}

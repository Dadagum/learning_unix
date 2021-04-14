#include <ctype.h>
#include <limits.h>
#include "print_wait_status.h" /* For printWaitStatus() */
#include "tlpi_hdr.h"

#define POPEN_FMT "/bin/ls -d %s 2> /dev/null"
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

// pipe:  execute a shell command and either read its output or send it some input
// 使用 popen 的例子：不断读取文件名匹配的模式（filename wildcard pattern），然后使用 popen 去获取将这个 pattern 传递给 ls 命令得到的结果

int main(int argc, char *argv[])
{
    char pat[PAT_SIZE]; /* Pattern for globbing */
    char popenCmd[PCMD_BUF_SIZE];
    FILE *fp;           /* File stream returned by popen() */
    Boolean badPattern; /* Invalid characters in 'pat'? */

    int len, status, fileCnt, j;
    char pathname[PATH_MAX];

    for (;;)
    { 
        printf("pattern: ");
        fflush(stdout);
        // 读取 pattern
        if (fgets(pat, PAT_SIZE, stdin) == NULL)
            break; /* EOF */
        len = strlen(pat);
        if (len <= 1) /* Empty line */
            continue;
        if (pat[len - 1] == '\n') /* Strip trailing newline */
            pat[len - 1] = '\0';
        // 校验 pattern（查看是否有非法字符）
        for (j = 0, badPattern = FALSE; j < len && !badPattern; j++)
            if (!isalnum((unsigned char)pat[j]) &&
                strchr("_*?[^-].", pat[j]) == NULL)
                badPattern = TRUE;
        if (badPattern)
        {
            printf("Bad pattern character: %c\n", pat[j - 1]);
            continue;
        }
        // 将 pat 按照 format 格式化成字符串并存放在 popenCmd 中
        snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat);
        popenCmd[PCMD_BUF_SIZE - 1] = '\0';

        // 执行 ls 命令，并准备读取 ls 命令的结果
        fp = popen(popenCmd, "r");
        if (fp == NULL)
        {
            printf("popen() failed\n");
            continue;
        }
        // 打印结果
        fileCnt = 0;
        while (fgets(pathname, PATH_MAX, fp) != NULL)
        {
            printf("%s", pathname);
            fileCnt++;
        }
        
        // 关闭和打印退出状态
        status = pclose(fp);
        printf(" %d matching file%s\n", fileCnt, (fileCnt != 1) ? "s" : "");
        printf(" pclose() status == %#x\n", (unsigned int)status);
        if (status != -1)
            printWaitStatus("\t", status);
    }
    exit(EXIT_SUCCESS);
}
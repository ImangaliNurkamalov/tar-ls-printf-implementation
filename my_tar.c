#include "my_tar.h"

// implement strlen function
int my_strlen(char* str)
{
    int runner = 0;

    while (str[runner] != 0)
    {
        runner++;
    }
    return runner;
}

// implement strcmp function
int my_strcmp(char* str1, char* str2)
{
    for (int i = 0;; i++)
    {
        if (str1[i] != str2[i])
        {
            return str1[i] < str2[i] ? -1 : 1;
        }

        if (str1[i] == '\0')
        {
            return 0;
        }
    }
}

// implement strncmp function
int my_strncmp(char* str1, char* str2, size_t n)
{
    while (n && *str1 && (*str1 == *str2))
    {
        str1 += 1;
        str2 += 1;
        n -= 1;
    }
    if (n == 0)
    {
        return 0;
    }
    else
    {
        return (*(unsigned char*)str1 - *(unsigned char*)str2);
    }
}

// implement strstr function
char* my_strstr(char* s1, char* s2, int size)
{
    int index = 0, runner = 0, helper;

    while (index < size)
    {
        helper = index;

        while (s2[runner] != '\0')
        {
            if (s1[helper] != s2[runner])
            {
                break;
            }
            helper += 1;
            runner += 1;
        }

        if (s2[runner] == '\0')
        {
            return (char*)s1 + index;
        }
        else
        {
            index += 1;
            runner = 0;
        }
    }
    return 0;
}

// function to check if a file exist
int c_exists(char* path)
{
    struct stat buffer;
    int exist = stat(path, &buffer);
    // if exist return 1
    if (exist == 0)
    {
        return 1;
    }
    // if doesn't exist return 0
    else
    {
        return 0;
    }
}

// function to get length of a file
int get_file_len(char* path)
{
    struct stat fileinfo;
    stat(path, &fileinfo);
    int size = fileinfo.st_size;
    return size;
}

// function to read a whole file
char* read_file(char* path, int fd)
{
    char* ret = malloc(get_file_len(path));
    read(fd, ret, get_file_len(path));
    return ret;
}

// converter
char* my_convert_for_header(char* res, unsigned int num, int size, int base)
{
    char sym[] = "0123456789abcdef";
    memset(res, '0', size - 1);
    res[size - 1] = '\0';
    size--;

    while (num)
    {
        res[--size] = sym[num % base];
        num /= base;
    }
    return res;
}

header* mem_header(int ac)
{
    int index = 3;
    while (index < ac)
    {
        index++;
    }
    int ans = index - 2;
    int ans_for_memory = 512 * ans;
    header* head = malloc(ans_for_memory);
    if (head != NULL)
    {
        bzero(head, ans_for_memory);
    }
    return head;
}

// function to get heade of a file
header* get_header(int ac, char* filename)
{
    struct stat file_info;

    if (stat(filename, &file_info) < 0)
    {
        printf("No such file/directory: %s\n", filename);
        return NULL;
    }
    header* head = mem_header(ac);
    strncpy(head->name, filename, 100);
    my_convert_for_header(head->mode, file_info.st_mode, 8, 8);
    my_convert_for_header(head->uid, file_info.st_uid, 8, 8);
    my_convert_for_header(head->gid, file_info.st_gid, 8, 8);
    my_convert_for_header(head->size, file_info.st_size, 12, 8);
    my_convert_for_header(head->mtime, file_info.st_mtime, 12, 8);
    my_convert_for_header(head->chksum, get_checksum(filename, 512), 8, 8);
    //printf("%o", get_checksum(filename, 512));
    head->typeflag = get_typerflag(file_info.st_mode);
    int len = readlink(filename, head->linkname, 100);
    strncpy(head->magic, TMAGIC, 6);
    strncpy(head->version, TVERSION, 2);
    struct passwd* pws;
    pws = getpwuid(file_info.st_uid);
    strncpy(head->uname, pws->pw_name, 32);
    struct group* grp;
    grp = getgrgid(file_info.st_gid);
    strncpy(head->gname, grp->gr_name, 32);
    char tempMajor[8];
    bzero(tempMajor, 8);
    char tempMinor[8];
    bzero(tempMinor, 8);
    strncpy(head->devmajor, my_convert_for_header(tempMajor, major(file_info.st_dev), 8, 8), 8);
    strncpy(head->devmajor, my_convert_for_header(tempMajor, major(file_info.st_dev), 8, 8), 8);
    char tempPrefix[155];
    bzero(tempPrefix, 155);
    strncpy(head->prefix, tempPrefix, 155);
    return head;
}

// function to read the tar
int read_tar(int ac, char** av, int option)
{
    if (ac < 3)
    {
        printf("Error!\n");
    }
    int fd = open(av[2], O_RDONLY);
    struct stat file;
    header* head;
    stat(av[2], &file);
    int size = file.st_size;
    char* ans = malloc(size);
    int res;
    read(fd, ans, size);
    for (int index = 3; index < ac; index++)
    {
        struct stat inform;
        stat(av[index], &inform);
        head = get_header(ac, av[index]);
        if (my_strstr(ans, head->name, size) && my_strstr(ans, head->size, size) && my_strstr(ans, head->mtime, size))
        {
            res = 0;
        }
        else
        {
             res = -1;
        }
        free_head(head);
    }
    free(ans);
    close(fd);
    return res;
}


// function to get typerflag
char get_typerflag(int mode)
{
    switch (mode & S_IFMT)
    {
        case S_IFREG:
            return REGTYPE;
        case S_IFLNK:
            return SYMTYPE;
        case S_IFCHR:
            return CHRTYPE;
        case S_IFBLK:
            return BLKTYPE;
        case S_IFDIR:
            return DIRTYPE;
        case S_IFIFO:
            return FIFOTYPE;
        default:
            return REGTYPE;
    }
}

// function to get checksum of a file
int get_checksum(char* buffer, int len)
{
    int res = 0;
    char* buf = buffer;

    for (int index = 0; index < len; index++)
    {
        res += *buf++;
    }
    return res;
}


// function to free linked list
void free_head(header* head)
{
    while (head)
    {
        header* iter = head;
        head = head->next;
        free(iter);
    }
}

// -c option
int opt_c(int ac, char** av)
{
    int fd;
    char* ans;
    char* ren;
    struct stat info;
    header* head_for_dir;
    //int res = 0;
    header* head;
    // if we have not enough parameters
    if (ac < 3)
    {
        write(1, "Error! Not enough parameters(?)\n", 32);
        return -1;
    }
    // can't create empty archive
    if (ac == 3)
    {
        write(1, ":tar: Cowardly refusing to create an empty archive\n", 51);
        return -1;
    }
    // if we have enough parameters
    else if (ac > 3)
    {
        fd = open(av[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
        for (int index = 3; index < ac; index++)
        {
            if (c_exists(av[index]) == 1)
            {
                stat(av[index], &info);
                if (S_ISDIR(info.st_mode) == true)
                {
                    char* file_content = malloc(sizeof(char*) * 512);
                    DIR* dp = opendir(av[index]);
                    struct dirent* dirp;
                    while ((dirp = readdir(dp)) != NULL)
                    {
                        if (my_strncmp(dirp->d_name, ".", 1))
                        {
                            sprintf(file_content, "%s/%s", av[index], dirp->d_name);
                            head_for_dir = get_header(ac, file_content);
                            int xd = open(file_content, O_RDONLY);
                            ren = read_file(file_content, xd);
                            write(fd, head_for_dir, 512);
                            write(fd, ren, get_file_len(file_content));
                            char end[BLOCKSIZE * 2];
                            bzero(end, BLOCKSIZE * 2);
                            write(fd, end, BLOCKSIZE * 2);
                            free(ren);
                        }
                    }
                    free(file_content);
                    free_head(head_for_dir);
                    closedir(dp);
                }
                else
                {
                    int file = open(av[index], O_RDONLY);
                    head = get_header(ac, av[index]);
                    write(fd, head, 512);
                    ans = read_file(av[index], file);
                    write(fd, ans, get_file_len(av[index]));
                    char end[BLOCKSIZE * 2];
                    bzero(end, BLOCKSIZE * 2);


                    write(fd, end, BLOCKSIZE * 2);
                    free(ans);
                    free_head(head);
                }
            }
            else if (c_exists(av[index]) == 0)
            {
                printf("tar: %s: Cannot stat: No such file or directory\n", av[index]);
            }
        }
    }
    return 0;
}

// function to get options(-c -r and etc)
void get_opt(int ac, char** av)
{
    // if we have -cf option
    if (my_strcmp(av[1], "-cf") == 0)
    {
        opt_c(ac, av);
    }
    // if we have -c option without -f option
    if (my_strcmp(av[1], "-c") == 0)
    {
        write(1, "Error missing -f option!\n", 26);
    }
    if (my_strcmp(av[1], "-r") == 0)
    {
        write(1, "Error missing -f option!\n", 26);
    }
    // if we have -rf option
    if (my_strcmp(av[1], "-rf") == 0)
    {
        opt_r(ac, av);
    }
    // if we have -uf option
    if (my_strcmp(av[1], "-uf") == 0)
    {
        opt_u(ac, av);
    }
    if (my_strcmp(av[1], "-u") == 0)
    {
        write(1, "Error missing -f option!\n", 26);
    }
    if (my_strcmp(av[1], "-xf") == 0)
    {
        //opt_x(ac, av);
    }
    if (my_strcmp(av[1], "-x") == 0)
    {
        write(1, "Error missing -f option!\n", 26);
    }
    if (my_strcmp(av[1], "-tf") == 0)
    {
        opt_t(ac, av);
    }
    if (my_strcmp(av[1], "-t") == 0)
    {
        write(1, "Error missing -f option!\n", 26);
    }

}

// -r option
int opt_r(int ac, char** av)
{
    int fd;
    int res;
   // struct stat file_info;
    header* head;
    char* ans;
    // if we have not enough parameters
    if (ac <= 3)
    {
        write(1, "Error! Not enough parameters(?)\n", 32);
        return -1;
    }
    // if we have enough parameters
    else
    {
        // if we have at least one parameter to add
        if (ac > 3)
        {
            // if the file after option doesn't exist create this file
            if (c_exists(av[2]) == 0)
            {
                opt_c(ac, av);
                return 0;
            }
            else if (c_exists(av[2]) == 1)
            {
                fd = open(av[2], O_APPEND | O_WRONLY, 0644);
                for (int runner = 3; runner < ac; runner++)
                {
                    if (c_exists(av[runner]) == 1)
                    {
                        int file_parameter = open(av[runner], O_RDONLY, 0644);
                        ans = read_file(av[runner], file_parameter);
                        head = get_header(ac, av[runner]);
                        write(fd, head, 512);
                        write(fd, ans, get_file_len(av[runner]));
                        char end[BLOCKSIZE * 2];
                        bzero(end, BLOCKSIZE * 2);
                        write(fd, end, BLOCKSIZE * 2);
                        free(ans);
                        free_head(head);
                        res = 0;
                    }
                    else if (c_exists(av[runner]) == 0)
                    {
                        printf("tar: %s: Cannot stat: No such file or directory\n", av[runner]);
                        res = -1;
                    }
                }
            }
        }
    }
    return res;
}

// -u option
int opt_u(int ac, char** av)
{
    int res;
    // if we have not enough parameters
    if (ac <= 3)
    {
        write(1, "Error! Not enough parameters(?)\n", 32);
        return -1;
    }
    // if we have enough parameters
    else
    {
        for (int runner = 3; runner < ac; runner++)
        {
            if (c_exists(av[runner]) == 1)
            {
                // if the file isn't inside archive or the mtime changed
                if (read_tar(ac, av, 1) == -1)
                {
                    opt_r(ac, av);
                }
                res = 0;
            }
            // if the one of the parameters don't exist at all
            else if (c_exists(av[runner]) == 0)
            {
                printf("tar: %s: Cannot stat: No such file or directory\n", av[runner]);
                res = -1;
            }
        }
    }
    return res;
}

// -t option

int opt_t(int ac, char** av)
{
    int res = 0;
    int fd = open(av[2], O_RDONLY);
    struct stat file;
    stat(av[2], &file);
    int size = file.st_size;
    char* ans = malloc(size);
    DIR* dp = opendir(".");
    struct dirent* dir;
    header* hed;
    read(fd, ans, size);
    while ((dir = readdir(dp)) != NULL)
    {
        hed = get_header(ac, dir->d_name);
        if (my_strstr(ans, hed->name, size) && my_strstr(ans, hed->size, size))
        {
            write(1, hed->name, 100);
            write(1, "\n", 1);
        }
        free_head(hed);
    }
    free(ans);
    closedir(dp);
    return res;
}
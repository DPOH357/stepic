#include <stdio.h>
#include <unistd.h>

const unsigned int max_commands = 16;
const unsigned int max_command_length = 32;

unsigned int parce_command_list(const char* command_list_str, char commands_list[max_commands][max_command_length])
{
    const unsigned int max_command_list_length = max_commands * max_command_length;
    unsigned int i;
    unsigned int num_command = 0;
    unsigned int icommand = 0;


    for(i = 0; i < max_command_list_length; i++)
    {
        char ch = command_list_str[i];
        switch(command_list_str[i])
        {
            case 0:
            {
                commands_list[num_command][icommand] = 0;
                return num_command;
            }
            break;

            case '|':
            {
                commands_list[num_command][icommand] = 0;
                num_command++;
                icommand = 0;
            }
            break;

            default:
            {
                commands_list[num_command][icommand] = command_list_str[i];
                icommand++;
            }
            break;
        }
    }

    return num_command;
}

unsigned int parce_command(const char* command, char params[2][max_command_length])
{
    unsigned int i;
    unsigned int iparam = 0;
    unsigned int num_param = 0;
    int b_string = 0;

    params[0][0] = params[1][0] = 0;

    for(i = 0; i < max_command_length; i++)
    {
        switch(command[i])
        {
        case 0:
        {
            params[num_param][iparam] = 0;
            return num_param;
        }
        break;

        case ' ':
        {
            if(iparam > 0 && !b_string)
            {
                params[num_param][iparam] = 0;
                num_param++;
                if(num_param >= 2)
                {
                    return 1;
                }
                iparam = 0;
            }
        }
        break;

        case '\"':
        {
            if(b_string) b_string = 0;
            else b_string = 1;
        }

        default:
        {
            params[num_param][iparam] = command[i];
            iparam++;
        }
        break;
        }
    }
}

void do_command(char params[2][max_command_length])
{
    if(params[1][0] == 0)
    {
        execlp(params[0], params[0], NULL);
    }
    else
    {
        execlp(params[0], params[0], params[1], NULL);
    }
}

void run_commands(unsigned int num_command, const char commands_list[max_commands][max_command_length], int to_file)
{
    if(to_file)
    {
        FILE* f = fopen("/home/box/result.out", "w");
        if(f)
        {
            close(STDOUT_FILENO);
            dup2(f->_fileno, STDOUT_FILENO);
            fclose(f);
        }
    }

    if(num_command > 0)
    {
        int pfd[2];
        pipe(pfd);

        if(!fork())
        {
            // child
            close(STDOUT_FILENO);
            dup2(pfd[1], STDOUT_FILENO);

            close(pfd[1]);
            close(pfd[0]);

            run_commands(num_command-1, commands_list, 0);
        }
        else
        {
            // parent
            close(STDIN_FILENO);
            dup2(pfd[0], STDIN_FILENO);

            close(pfd[1]);
            close(pfd[0]);

            char params[2][max_command_length];
            parce_command(commands_list[num_command], params);
            do_command(params);
        }
    }
    else
    {
        char params[2][max_command_length];
        parce_command(commands_list[num_command], params);
        do_command(params);
    }
}

int main(int argv, char** argc)
{
	const unsigned int command_line_length = max_commands*max_command_length;
    char commands_line[command_line_length];
    //gets(commands_line);
    //sprintf(commands_line, "who | sort | uniq -c | sort -nk1");
	fgets(commands_line, command_line_length, stdin);

    FILE* flog = fopen("log.txt", "a+");
    if(flog)
    {
        fprintf(flog, "%s\n", commands_line);
        fclose(flog);
    }

    char command_list[max_commands][max_command_length];
    unsigned int num_current_command = parce_command_list(commands_line, command_list);

    run_commands(num_current_command, command_list, 1);

    return 0;
}

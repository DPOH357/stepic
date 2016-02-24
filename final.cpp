#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <functional>
#include <thread>
#include <string.h>
#include <vector>
#include <string>

namespace srv
{


struct parameters
{
    std::string ip;
    std::string port;
    std::string directory;
};

} // namespace srv

void parce_request(const char* request, unsigned short size, std::vector<std::string>& string_list)
{
    string_list.clear();
    string_list.push_back(std::string());
    for(unsigned short i = 0; i < size; ++i)
    {
        switch(request[i])
        {
        case ' ':
        {
            string_list.push_back(std::string());
        }
        break;

        case 0:
        {
            i = size;
        }
        break;

        default:
        {
            string_list.back().push_back(request[i]);
        }
        break;
        }
    }
}

bool generate_response(const std::vector< std::string >& string_list,
                       const srv::parameters& params,
                       std::string& out_response)
{
    out_response.clear();

    unsigned short str_num(0);

    for(auto str : string_list)
    {
        if(str.compare("GET") == 0)
        {
            break;
        }
        str_num++;
    }

    if(str_num < string_list.size() - 1)
    {
        int pos(0);
        std::string file_str = string_list[str_num + 1];
        std::string address(params.ip + ":" + params.port + "/");
        pos = file_str.find(address);
        if(pos >= 0)
        {
            file_str = file_str.substr(pos + address.length());
        }
        address = "localhost:" + params.port + "/";
        pos = file_str.find(address);
        if(pos >= 0)
        {
            file_str = file_str.substr(pos + address.length());
        }
        address = "localhost/";
        pos = file_str.find(address);
        if(pos >= 0)
        {
            file_str = file_str.substr(pos + address.length());
        }

        std::string path_file = params.directory + file_str;
        pos = path_file.find('?');
        if(pos >= 0)
        {
            path_file.resize(pos);
        }
        pos = path_file.find('\r');
        if(pos >= 0)
        {
            path_file.resize(pos);
        }
        FILE* file = fopen(path_file.c_str(), "r");
        if(file)
        {
            char ch;
            fscanf(file, "%c", &ch);

            if(ch != 0)
            {
                out_response = "HTTP/1.1 200 OK\r\n";

                std::string text;
                while(!feof(file))
                {
                    text.push_back(ch);
                    fscanf(file, "%c", &ch);
                }

                out_response += "Content-Length: ";
                out_response += std::to_string(text.length()) + "\r\n";

                out_response += "Connection: close\r\n";

                out_response += "Content-Type: text/html\r\n";

                out_response += "Accept-Ranges: bytes\r\n";
                out_response += "\r\n";
                out_response += text;
            }
            else
            {
                out_response = "HTTP/1.1 404 Not Found\r\n";
                out_response += "Content-Type: text/html\r\n";
            }

            fclose(file);
        }
        else
        {
            out_response = "HTTP/1.1 404 Not Found\r\n";
            out_response += "Content-Type: text/html\r\n";
        }

        return true;
    }

    return false;
}

void run_client_manager(int socket_slave, srv::parameters params)
{
    //printf("Run client manager id: %d\n", (int)std::this_thread::get_id());

    const unsigned short buffer_size(2048);
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);

    while(true)
    {
        size_t recv_bytes = recv(socket_slave, (void*)buffer, buffer_size - 1, MSG_NOSIGNAL);
        if(recv_bytes > 0)
        {
            std::vector<std::string> string_list;
            parce_request(buffer, buffer_size, string_list);

            std::string response;
            if(generate_response(string_list, params, response))
            {
                send(socket_slave, response.data(), response.length(), MSG_NOSIGNAL);
            }
        }
        else
        {
            //printf("Close client manager id: %d\n", (unsigned int)std::this_thread::get_id());

            shutdown(socket_slave, SHUT_RDWR);
            close(socket_slave);
            break;
        }
    }
}

void run_master(struct sockaddr_in sock_addr, srv::parameters params)
{
    int socket_master =
            socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socket_master == -1)
    {
        return;
    }

    bind(socket_master, (struct sockaddr*) &sock_addr, sizeof(sock_addr));

    listen(socket_master, SOMAXCONN);

    while(true)
    {
        int socket_slave = accept(socket_master, 0, 0);

        std::thread thread(run_client_manager, socket_slave, params);
        thread.detach();
    }
}

int main(int argc, char** argv)
{
    srv::parameters params;

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;

    const char* options = "h:p:d:";
    int opt = getopt(argc, argv, options);
    while(opt != -1)
    {
        switch(opt)
        {
        case 'h': params.ip = optarg; break;
        case 'p': params.port = optarg; break;
        case 'd': params.directory = optarg; break;
        default: return 1;
        }
        opt = getopt(argc, argv, options);
    }

    inet_pton(AF_INET, params.ip.c_str(), &sock_addr.sin_addr.s_addr);
    sock_addr.sin_port = htons( atoi(params.port.c_str()) );

    if(fork() == 0)
    {
        umask(0);
        setsid();

        FILE* file = fopen("masterpid.txt", "w");
        if(file)
        {
            fprintf(file, "%d\n", (unsigned int)getpid());
            fclose(file);
        }

        run_master(sock_addr, params);
    }
    return 0;
}

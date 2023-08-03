#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <thread>
#include <ctime>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <unordered_map>

const std::string SERVER_IP = "0.0.0.0";
//const char* key = generate_key();
int PORT = 5050;

int	ft_atoi(const char* a)
{
    int	ret;
    int	sign;

    ret = 0;
    sign = 1;
    while ((*a >= 9 && *a <= 13) || *a == 32)
    {
        a++;
    }
    if (*a == '+' || *a == '-')
    {
        if (*a == '-')
            sign = -1;
        a++;
    }
    while (*a >= '0' && *a <= '9')
    {
        ret = ret * 10 + (*a - '0');
        a++;
    }
    return (sign * ret);
}
class db
{
public:
    db() {}

    db(std::string name, std::vector<std::string> initial_values)
        :name_(name), header(initial_values)
    {}

    void rename(std::string new_name)
    {
        name_ = new_name;
    }
    
    void add_value(std::vector<std::string> new_value)
    {
        values.push_back(new_value);
       for (int i = 0; i < header.size(); i++)
       {
            if (querying_cache.contains(header[i]) == true) 
            {
                querying_cache[header[i]].insert({new_value[i], {i, size}});   
            }
            else
            {
                querying_cache.insert({header[i], {}});
                querying_cache[header[i]].insert({new_value[i], {i, size}});
            }
       }
        size++;
    }

    void remove_value(int index)
    {
        values.erase(values.begin() + index);
    }

    std::vector<std::vector<std::string>> get_values()
    {
        return values;
    }

    std::vector<std::string> get_value(int index)
    {
        return values[index];
    }

    std::string get_name()
    {
        return name_;
    }

    int get_size()
    {
        return size;
    }

    int header_size()
    {
        return header.size();
    }
    std::vector<std::string> get_header()
    {
        return header;
    }
private:
    std::string name_;
    std::vector<std::string> header;
    std::vector<std::vector<std::string>> values;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> querying_cache;
    int size = 0;

};

std::vector<db> dbs;

std::vector<std::string> tokenize(std::string result, char separator = ' ')
{
    std::vector<std::string> tokens;
    int index = 0;
    int a = 0;
    bool inside = false;
    size_t last = 0;
    while (result[index] == ' ' || result[index] == separator)
        index++;
    if (index > -1)
        index--;
    if (index >= 0)
    {
        a = index;
        last = index + 1;
    }
    while (result[a])
    {
        index++;
        a = index;
        if (result[index] == separator || result[index] == '"' || result[index] == '\0')
        {
            if ((result[index] == separator || result[index] == '\0') && inside == false)
            {
                char* buffer = (char*)calloc(static_cast<size_t>(index - last) + 1, sizeof(char));
                if (!buffer)
                    return {};
                memcpy(buffer, &result.c_str()[last], index - last);
                std::string ret = buffer;
                if (ret.size() == 0)
                    break;
                tokens.push_back(ret);
                //last = index;
                while (result[index] == ' ' || result[index] == separator)
                    index++;
                if (result[index] == '"')
                    index--;
                last = index;
                free(buffer);
            }
            else if (result[index] == '"' && inside == false)
            {
                last = index;
                inside = true;
            }
            else if (result[index] == '"' && inside == true)
            {
                last++;
                char* buffer = (char*)calloc(static_cast<size_t>(index - last) + 1, sizeof(char));
                if (!buffer)
                    return {};
                memcpy(buffer, &result.c_str()[last], index - last);
                std::string ret = buffer;
                if (ret.size() == 0)
                    break;
                tokens.push_back(ret);
                index++;
                if (result[index] != '\0')
                {
                    while (result[index] == ' ' || result[index] == separator)
                        index++;
                    if (result[index] == '"')
                        index--;
                    last = index;
                }
                else
                    break;
                free(buffer);
                inside = false;
            }
            else if (result[index] == '\0' && inside == true)
            {
                char* buffer = (char*)calloc(static_cast<size_t>(index - last) + 1, sizeof(char));
                if (!buffer)
                    return {};
                memcpy(buffer, &result.c_str()[last], index - last);
                std::string ret = buffer;
                if (ret.size() == 0)
                    break;
                tokens.push_back(ret);
                free(buffer);
                inside = false;
            }
        }
    }
    return tokens;
}

void print_line(std::vector<std::string> header)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "+";
        std::cout << "-";
        for (unsigned int x = 0; x < header[i].size(); x++)
            std::cout << "-";
        std::cout << "-";
    }
    std::cout << "+";
    std::cout << std::endl;
}

void print_line(std::vector<std::string> header, std::vector<int> max)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "+";
        std::cout << "-";
        for (unsigned int x = 0; x < max[i]; x++)
            std::cout << "-";
        std::cout << "-";
    }
    std::cout << "+";
    std::cout << std::endl;
}

void print_words(std::vector<std::string> header)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "|" << " " << header[i] << " ";
    }
    std::cout << "|";
    std::cout << std::endl;
}

void print_words(std::vector<std::string> header, std::vector<int> max)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "|" << " " << header[i];
        for (unsigned int x = 0; x < (max[i] - (header[i].length() - 1)); x++)
            std::cout << " ";
    }
    std::cout << "|";
    std::cout << std::endl;
}

int vector_search(std::vector<std::string> vector, std::string str_to_search, int size = -1)
{
    if (size == -1)
        size = vector.size();
    for (unsigned int i = 0; i < size; i++)
    {
        if (vector[i].compare(str_to_search) == 0)
            return i;
    }
    return -1;
}

int vector_search(std::vector<std::string> vector, char str_to_search, int size = -1)
{
    if (size == -1)
        size = vector.size();
    for (unsigned int i = 0; i < size; i++)
    {
        if (vector[i].find(str_to_search) != -1)
            return vector[i].find(str_to_search);
    }
    return -1;
}

void evaluate(std::vector<std::string> tokens)
{
    if (tokens[0].compare("create") == 0)
    {
        if (tokens.size() < 3)
        {
            std::cout << "Peticion de creacion invalida" << std::endl;
            return;
        }
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << "Una base de datos con este nombre ya ha sido creada" << std::endl;
                return;
            }
        }
        std::vector<std::string> buffer;
        std::string write_file;
        for (unsigned int i = 2; i < tokens.size(); i++)
        {
            buffer.push_back(tokens[i]);
            write_file.append("\"");
            write_file.append(tokens[i]);
            write_file.append("\"");
            if (i < tokens.size() - 1)
                write_file.append(" ");
        }
        dbs.push_back(db(tokens[1], buffer));
        std::ofstream db(tokens[1].append(".ldb"));
        db << write_file.append("\n");
        std::cout << "La base de datos se ha creado correctamente" << std::endl;
        db.close();
    }

    if (tokens[0].compare("add") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                if (tokens.size() - 2 == dbs[i].header_size())
                {
                    std::vector<std::string> db_push;
                    std::string db_write;
                    for (unsigned int i = 2; i < tokens.size(); i++)
                    {
                        db_push.push_back(tokens[i]);
                        db_write.append("\"");
                        db_write.append(tokens[i]);
                        db_write.append("\"");
                        if (i < tokens.size() - 1)
                            db_write.append(" ");
                    }
                    dbs[i].add_value(db_push);
                    std::ofstream db_writee(tokens[1].append(".ldb"), std::ios_base::app);
                    db_writee << db_write.append("\n");
                }
                else
                {
                    std::cout << "La cantidad de datos introducidos es incorrecta" << std::endl;
                }
                return;
            }
        }
        std::cout << "No se ha encontrado una base de datos con ese nombre (ls para ver las bases de datos activas)" << std::endl;
    }

    if (tokens[0].compare("show") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::vector<int> max;
                std::vector<std::vector<std::string>> values = dbs[i].get_values();
                std::vector<std::string> header = dbs[i].get_header();
                for (unsigned int y = 0; y < header.size(); y++)
                {
                    max.push_back(header[y].length());
                    for (unsigned int x = 0; x < values.size(); x++)
                    {
                        if (max.empty() || (max.size() - 1) < y)
                        {
                            max.push_back(values[x][y].size());
                            continue;
                        }
                        if (values[x][y].size() > max[y])
                            max[y] = values[x][y].size();
                    }
                }
                print_line(header, max);
                print_words(header, max);
                print_line(header, max);
                for (unsigned int y = 0; y < values.size(); y++)
                {
                    print_words(values[y], max);
                    print_line(values[y], max);
                }

                return;
            }
        }
    }

    if (tokens[0].compare("ls") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            std::cout << dbs[i].get_name();
            if (i < dbs.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
    if (tokens[0].compare("size") == 0) //doesnt work
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << dbs[i].get_size() << std::endl;
                return;
            }
        }
        std::cout << "No se ha encontrado una base de datos con ese nombre (ls para ver las bases de datos activas)" << std::endl;
    }
    if (tokens[0].compare("load") == 0)
    {
        std::string name = tokens[1];
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << "Ya existe una base de datos con ese nombre (remame para cambiar el nombre)" << std::endl;
                return;
            }
        }
        if (std::filesystem::exists(tokens[1].append(".ldb")))
        {
            std::ifstream infile(tokens[1]);
            std::string head;
            std::getline(infile, head);
            //std::cout << head << std::endl;
            std::vector<std::string> header = tokenize(head);
            dbs.push_back(db(name, header));
            std::string linea;
            while (std::getline(infile, linea))
            {
                //std::cout << linea << std::endl;
                std::vector<std::string> values = tokenize(linea);
                dbs.back().add_value(values);
                //linea.clear();
            }
            return;
        }
        std::cout << "El archivo con ese nombre no fue encontrado recuerda que tiene que tener la extension .ldb" << std::endl;
    }
    if (tokens[0].compare("rename") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                dbs[i].rename(tokens[2]);
                std::rename(tokens[1].append(".ldb").c_str(), tokens[2].append(".ldb").c_str());
            }
        }
    }
    if (tokens[0].compare("get") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0 && dbs[i].get_size() > ft_atoi(tokens[2].c_str()) - 1)
            {
                if (tokens.size() > 2)
                {
                    if (tokens[2].compare("*") == 0)
                    {
                        std::vector<std::vector<std::string>> values = dbs[i].get_values();
                        int size = dbs[i].header_size();
                        for (unsigned int y = 0; y < values.size(); y++)
                        {
                            for (unsigned int x = 0; x < (unsigned int)size; x++)
                            {
                                std::cout << values[y][x];
                                if (x < (unsigned int)size - 1)
                                    std::cout << " ";
                            }
                            std::cout << std::endl;
                        }
                        break;
                    }
                    int search_index = vector_search(tokens, "where");
		    std::cout << search_index << std::endl;
                    if (search_index > -1)
                    {
                        std::cout << search_index << std::endl;
			if (tokens[search_index + 1].find('=') != std::string::npos)
			{
				std::vector<std::string> query = tokenize(tokens[search_index + 1], '=');
				std::vector<std::string> header = dbs[i].get_header();
				int index = -1;
				for (unsigned int x = 0; x < header.size(); x++)
				{
					if (header[x].compare(query[0]) == 0)
					{
						index = x;
						break;
					}
				}
				if (index == -1)
				{
					std::cout << "no se ha encontrado la columna que quieres buscar" << std::endl;
					break;
				}
				std::cout << header[index] << std::endl;
				std::vector<std::vector<std::string>> values = dbs[i].get_values();
				for (unsigned int x = 0; x < dbs[i].get_size(); x++)
				{
					if (values[x][index].compare(query[1]) == 0)
					{
						for (unsigned int y = 0; y < header.size(); y++)
						{
							std::cout << values[x][y];
							if (y < tokens.size() - 1)
								std::cout << " ";
						}
						std::cout << std::endl;
					}
				}
			}
			else
				std::cout << "No hay =" << std::endl;
			break;
                    }
                    std::vector<std::string> values = dbs[i].get_value(ft_atoi(tokens[2].c_str()) - 1);
                    for (unsigned int x = 0; x < values.size(); x++)
                    {
                        std::cout << values[x];
                        if (x < tokens.size() - 1)
                            std::cout << " ";
                    }
                    std::cout << std::endl;
                }
                else
                    std::cout << "No hay suficientes parametros" << std::endl;
                return;
            }
        }
    }
}


void create_config()
{
    std::ofstream cfg("config.cfg");
    cfg << "//Esta es la configuracion de la base de datos\n";
    cfg << "//Esto cambia el puerto a donde mira el servidor por sockets de la base de datos (default 5049)\n";
    cfg << "port:5049";
    cfg.close();
}

void load_config()
{
    std::ifstream infile("config.cfg");
    std::string linea;
    while (std::getline(infile, linea))
    {
        //std::cout << linea << std::endl;
        if (linea.starts_with("//") == false)
        {
            std::vector<std::string> values = tokenize(linea, ':');
            if (values[0].compare("port") == 0)
            {
                PORT = atoi(values[1].c_str());
                break;
            }
        }
        //linea.clear();
    }
}

int main()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    if (std::filesystem::exists("config.cfg") == false)
    {
        create_config();
    }
    else
    {
        load_config();
    }
    char msg[1024];
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "> ";
        std::cin.getline(msg, 1024);
        if (msg[0] != NULL)
        {
            std::string test = msg;
            auto t1 = high_resolution_clock::now();
            std::vector<std::string> tokens = tokenize(test);
            auto t2 = high_resolution_clock::now();

            /* Getting number of milliseconds as an integer. */
            auto ms_int = duration_cast<milliseconds>(t2 - t1);

            /* Getting number of milliseconds as a double. */
            duration<double, std::milli> ms_double = t2 - t1;

            //std::cout << ms_int.count() << "ms\n";
            std::cout << ms_double.count() << "ms\n";
            evaluate(tokens);
        }
    }
    return 0;
}

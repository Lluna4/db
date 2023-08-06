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
#include <map>
#include <format>

const std::string SERVER_IP = "0.0.0.0";
//const char* key = generate_key();
int PORT = 5050;
bool debug = false;

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

static int	ft_intlen(int n)
{
	int	ret;

	ret = 1;
	while (n >= 10)
	{
		ret++;
		n = n / 10;
	}
	return (ret);
}

static char	*ft_make_ret(int n, int sign)
{
	int		len;
	char	*ret;

	len = ft_intlen(n) + sign;
	ret = (char *)calloc(len + 1, sizeof(char));
	if (!ret)
		return (0);
	len--;
	while (len >= 0)
	{
		ret[len] = (n % 10) + '0';
		n = n / 10;
		len--;
	}
	if (sign == 1)
		ret[0] = '-';
	return (ret);
}

char	*ft_itoa(int n)
{
	char	*ret;
	int		sign;

	sign = 0;
	if (n == -2147483648)
	{
		ret = (char *)malloc(12 * sizeof(char));
		if (!ret)
			return (0);
		memcpy(ret, "-2147483648", 12);
		return (ret);
	}
	if (n < 0)
	{
		n *= -1;
		sign = 1;
	}
	return (ft_make_ret(n, sign));
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
                if (querying_cache[header[i]].contains(new_value[i]) == true)
                {
                    if (overrides.contains(new_value[i]) == true)
                    {
                        int x = overrides[new_value[i]] + 1;
                        overrides[new_value[i]] = x;
                        std::cout << "A" << std::endl;
                    }
                    else
                        overrides.insert({new_value[i], 1});
                    std::cout << overrides[new_value[i]] << " " << new_value[i] << std::endl;
                    new_value[i].append(ft_itoa(overrides[new_value[i]]));
                }
                querying_cache[header[i]].insert({new_value[i], {i, size}});   
            }
            else
            {
                querying_cache.insert({header[i], {}});
                if (querying_cache[header[i]].contains(new_value[i]) == true)
                {
                    if (overrides.contains(new_value[i]) == true)
                    {
                        int x = overrides[new_value[i]] + 1;
                        overrides[new_value[i]] = x;
                    }
                    else
                        overrides.insert({new_value[i], 1});
                    std::cout << overrides[new_value[i]] << " " << new_value[i] << std::endl;
                    new_value[i].append(ft_itoa(overrides[new_value[i]]));
                }
                querying_cache[header[i]].insert({new_value[i], {i, size}});
            }
       }
        size++;
    }
    
    std::vector<std::vector<std::string>> search(std::string column, std::string value)
    {
        std::vector<std::vector<std::string>> ret;
        if (querying_cache.contains(column) == true)
        {
            if (querying_cache[column].contains(value) == true)
            {
                std::vector<int> pos = querying_cache[column][value];
                std::vector<std::string> add_ve = values[pos[1]];
                ret.push_back(add_ve);
                if (overrides.contains(value) == true)
                {
                   int loops = overrides[value];
                   std::string initial_value = value;
                   for (int i = 1; i <= loops; i++)
                   {
                        value = std::format("{}{}", initial_value, ft_itoa(i));
                        std::vector<int> posi = querying_cache[column][value];
                        std::vector<std::string> add_vec = values[posi[1]];
                        ret.push_back(add_vec);
                   }
                }
                return ret;
            }
        }
        return ret;
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

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> cache()
    {
        return querying_cache;
    }

private:
    std::string name_;
    std::vector<std::string> header;
    std::vector<std::vector<std::string>> values;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> querying_cache;
    std::map<std::string, int> overrides;
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
           if (dbs[i].get_name().compare(tokens[1]) == 0)
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
                    if (search_index > -1)
                    {
                        if (tokens[search_index + 1].find('=') != std::string::npos)
                        {
                            std::vector<std::string> query = tokenize(tokens[search_index + 1], '=');
                            std::vector<std::vector<std::string>> ret = dbs[i].search(query[0], query[1]);

                            for (int x = 0; x < ret.size(); x++)
                            {
                                for (int y = 0; y < ret[x].size(); y++)
                                {
                                    std::cout << ret[x][y] << " ";
                                }
                                std::cout << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (tokens[0].compare("query_cache") == 0 && debug == true)
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> querying;
        //std::cout << "a" << std::endl;
        if (tokens.size() < 2)
            return;
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                querying = dbs[i].cache();
                break;
            }
            std::cout << "No se encontro base de datos" << std::endl;
        }
        for (const auto & [ key, value ] : querying) 
        {
            std::cout << " column: " << key << " value :";
            for (const auto & [ key2, value2 ] : value) 
            {
                std::cout << key2 << " position : (" << value2[0] << ", "<< value2[1] << ")" << std::endl;
            }
        }
    }
    if (tokens[0].compare("search") == 0 && debug == true)
    {
        for (int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::vector<std::vector<std::string>> ret = dbs[i].search(tokens[2], tokens[3]);
                for (int x = 0; x < ret.size(); x++)
                {
                    for (int y = 0; y < ret[x].size(); y++)
                    {
                        std::cout << ret[x][y] << " ";
                    }
                    std::cout << std::endl;
                }
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

int main(int argc, char *argv[])
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    
    if (argc > 1)
    {
        //std::cout << "A" << argv[1] << std::endl;
        if (strcmp(argv[1], "--debug") == 0)
        {
            debug = true;
            std::cout << "debug activated" << std::endl;
        }
    }

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
        if (msg[0] != '\0')
        {
            std::string test = msg;
            std::vector<std::string> tokens = tokenize(test);
            auto t1 = high_resolution_clock::now();
            evaluate(tokens);
            auto t2 = high_resolution_clock::now();

            /* Getting number of milliseconds as an integer. */
            auto ms_int = duration_cast<milliseconds>(t2 - t1);

            /* Getting number of milliseconds as a double. */
            duration<double, std::milli> ms_double = t2 - t1;

            //std::cout << ms_int.count() << "ms\n";
            std::cout << ms_double.count() << "ms\n";
        }
    }
    return 0;
}

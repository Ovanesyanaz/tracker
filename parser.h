#ifndef IN_PARSER_H
#define IN_PARSER_H
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSVRow
{
    public:
        std::string operator[](std::size_t index) const
        {
            return std::string(&m_line[m_data[index] + 1], m_data[index + 1] -  (m_data[index] + 1));
        }
        std::size_t size() const
        {
            return m_data.size() - 1;
        }
        void readNextRow(std::istream& str)
        {
            std::getline(str, m_line);

            m_data.clear();
            m_data.emplace_back(-1);
            std::string::size_type pos = 0;
            while((pos = m_line.find(',', pos)) != std::string::npos)
            {
                m_data.emplace_back(pos);
                ++pos;
            }
            // This checks for a trailing comma with no data after it.
            pos   = m_line.size();
            m_data.emplace_back(pos);
        }
    private:
        std::string         m_line;
        std::vector<int>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}   

void get_vector_of_vector_double(std::vector<std::vector<double>> &vec, std::string filename)
{
    std::ifstream file(filename);
    CSVRow row;
    while(file >> row)
    {
        std::vector<double> buf;
        for(int i = 0; i < row.size(); i++){
            buf.push_back(std::stod(row[i]));
        }
        vec.push_back(buf);
    }
}

void vector_of_vector_double_to_csv(std::vector<std::vector<double>> data, std::string filename)
{
    std::ofstream myfile;
    myfile.open(filename);

    for (std::vector<double> itr : data){
        for (int i = 0; i < itr.size() - 1; i++){
            myfile << itr[i] << ",";
        }
        myfile << itr[itr.size() - 1] << std::endl;
    }
    myfile.close();
}

#endif
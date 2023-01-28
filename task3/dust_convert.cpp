#include "Header.h"
#include "check_string.h"

int check_err_file(ifstream &data_file, ofstream &hex_file)
{
    if (!data_file == 1)
    {
        return 1;
    }
    if (!hex_file == 1)
    {
        return 3;
    }
    return 0;
}

bool check_err_dataline(string word, int count1);
class byte_seq
{
public:
    const uint8_t start_byte = 0x00;
    uint8_t packet_byte;
    uint8_t id;
    uint32_t time;
    uint32_t PM25;
    uint16_t aqi;
    uint8_t checksum;
    const uint8_t end_byte = 0xFF;
    uint8_t bytes_ana[15];

    void analy_bytes()
    {
        this->bytes_ana[0] = this->start_byte;
        this->bytes_ana[1] = this->packet_byte;
        this->bytes_ana[2] = this->id;
        for (int i = 0; i < 4; i++)
        {
            this->bytes_ana[6 - i] = (this->time >> i * 8) & 0xFF;
            this->bytes_ana[10 - i] = (this->PM25 >> i * 8) & 0xFF;
            if (i < 2)
            {
                this->bytes_ana[12 - i] = (this->aqi >> i * 8) & 0xFF;
            }
        }
        this->bytes_ana[14] = this->end_byte;
    }

    void calc_checksum()
    {
        this->checksum = 0;
        for (int i = 1; i < 13; i++)
        {
            this->checksum += this->bytes_ana[i];
        }
        this->bytes_ana[13] = ~this->checksum + 1;
    }
};

union float_uint32
{
    float input;
    uint32_t ouput;
};

void import_data(int &count1, byte_seq &temp, struct tm &tm, string word);
int main(int argc, char **argv)
{
    string data_filename = argv[1], hex_filename = argv[2];

    byte_seq temp;
    struct tm tm;

    /* Open input file to read*/
    ifstream data_file;
    data_file.open(data_filename, ios::in);

    /* Open output file to write */
    ofstream hex_file;
    hex_file.open(hex_filename, ios::trunc);

    /* Open error file to write */
    ofstream error_file;
    error_file.open("task3.log", ios::trunc);

    /* Check error close open file */
    int check_file = check_err_file(data_file, hex_file);
    string list_err_file[] = {"No error file", "Error 01: file not found or cannot be accesse", "Error 02: invalid csv file", "Error 04: cannot override hex_filename.dat"};
    if (check_file != 0)
    {
        error_file << list_err_file[check_file];
        return 1;
    }

    /* Declare variable*/
    int count1;          // Index of data per line: 0 is id, 1 is time, 2 is values, 3 is aqi, 4 is pollution
    int count_line = 0;  // line index
    bool check_err_data; // check error of data
    string line, word;
    getline(data_file, line); // get title line

    while (!data_file.eof())
    {
        check_err_data = 1;
        getline(data_file, line); // get line
        count1 = 0;
        count_line++;
        temp.packet_byte = 4; // start_byte (1), packet_byte (1), Check_sum(1), Stop_byte (1)

        stringstream s(line);
        while (getline(s, word, ','))
        {
            check_err_data = check_err_dataline(word, count1);
            if (check_err_data == 0)
                break; // if data is invalid, exit the loop
            else
                // if data is valid, store data to object temp
                import_data(count1, temp, tm, word);
        }
        if (check_err_data == 0)
            error_file << "Error 03: data is missing at line " << count_line << endl;
        else
        {
            temp.analy_bytes();
            temp.calc_checksum(); // calc check sum
            for (int i = 0; i < temp.packet_byte; i++)
                hex_file << uppercase << hex << std::setfill('0') << std::setw(2) << (unsigned)temp.bytes_ana[i] << " ";
            hex_file << endl;
        }
    }
    return 0;
}
bool check_err_dataline(string word, int count1)
{
    // return 0 if (string) word is invalid data
    if (isBlank(word) == 1)
        return 0;
    switch (count1)
    {
    case 0:
        return isPosIntNumber(word);
    case 1:
        return isTime(word);
    case 2:
        return isPosRealNumber(word);
    case 3:
        return isPosIntNumber(word);
    default:
        return 1;
    }
}
void import_data(int &count1, byte_seq &temp, struct tm &tm, string word)
{
    union float_uint32 data;
    switch (count1)
    {
    case 0:
        temp.id = stoi(word);
        temp.packet_byte += sizeof(temp.id);
        break;
    case 1:
    {
        std::istringstream ss(word);
        ss >> std::get_time(&tm, "%Y:%m:%d %H:%M:%S");
        temp.time = mktime(&tm);
        temp.packet_byte += sizeof(temp.time);
        break;
    }

    case 2:
        data.input = stof(word);
        temp.PM25 = data.ouput;
        temp.packet_byte += sizeof(temp.PM25);
        break;
    case 3:
        temp.aqi = stoi(word);
        temp.packet_byte += sizeof(temp.aqi);
        break;
    default:
        break;
    }
    count1 = (count1 + 1) % 5;
}
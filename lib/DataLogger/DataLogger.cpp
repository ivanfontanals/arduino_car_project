#include "DataLogger.h"
#include <Constants.h>

#define FILE_BASE_NAME "Data"
// SD chip select pin.  Be sure to disable any other SPI devices such as Enet.
const uint8_t chipSelect = SS;

//==============================================================================
// Error messages stored in flash.
#define error(msg) this->sd.errorHalt(F(msg))
//------------------------------------------------------------------------------

#define WRITE_FLAGS O_WRONLY | O_CREAT | O_EXCL

void DataLogger::init() {
    if (DEBUG) 
        Serial.println("Initializing SD card for WRITE");
    init_sd_card();
    char* filename = get_filename();
    
    if (DEBUG) {
        Serial.print("Writing to file ");
        Serial.println(filename);
    }
    if (!this->file.open(filename, WRITE_FLAGS)) {
        error("file.open");
    }
}

void DataLogger::init_sd_card() {
    // Initialize at the highest speed supported by the board that is
    // not over 50 MHz. Try a lower speed if SPI errors occur.
    if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
        sd.initErrorHalt();
    }
}

void DataLogger::read(char* source) {
    struct CarDataRecord myData_read;
    SdFile input;

    //init_sd_card();

    if (!input.open(source, O_RDONLY)) {
        error("Error opening input file");
    }

    while (input.available()) {
        struct CarDataRecord myData_read;
        input.read((uint8_t*)&myData_read, sizeof(myData_read));
        Serial.print(myData_read.millis);
        Serial.print(";");
        Serial.print(myData_read.us_center);
        Serial.print(";");
        Serial.print(myData_read.us_left);
        Serial.print(";");
        Serial.print(myData_read.us_right);
        Serial.println(";");
    }
}

void DataLogger::convert_to_csv(char* source, char* destination) {
    struct CarDataRecord record;
    SdFile csv, input;
    int sd_buffer_size = 0;

    init_sd_card();

    if (!input.open(source, O_RDONLY)) {
        error("Error opening input file");
    }

    if (!csv.open(destination, O_WRONLY | O_CREAT)) {
        error("Error opening destination CSV file");
    }

    Serial.println("Converting binary file to CSV.....");
    while (input.available()) {
        Serial.print(".");
        char buffer[50];

        input.read((uint8_t*)&record, sizeof(record));
        int row_size = sprintf(buffer, "%u,%u,%u,%u\r\n", record.millis, record.us_center, record.us_left, record.us_right);

        if (sd_buffer_size + row_size >= 512) {
            csv.flush();
            sd_buffer_size = 0;
        }

        sd_buffer_size += row_size;
        csv.write(buffer);
        Serial.print(";");
    }
    csv.close();
    Serial.println("Done!");
    SysCall::halt();
}

char* DataLogger::get_filename() {
    const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
    char fileName[13] = FILE_BASE_NAME "00.bin";

    // Find an unused file name.
    if (BASE_NAME_SIZE > 6) {
        error("FILE_BASE_NAME too long");
    }

    while (this->sd.exists(fileName)) {
        if (fileName[BASE_NAME_SIZE + 1] != '9') {
            fileName[BASE_NAME_SIZE + 1]++;
        } else if (fileName[BASE_NAME_SIZE] != '9') {
            fileName[BASE_NAME_SIZE + 1] = '0';
            fileName[BASE_NAME_SIZE]++;
        } else {
            error("Can't create file name");
        }
    }

    char* output = (char*)malloc(13);
    strcpy(output, fileName);
    return output;
}

/**
 * Set DC motor A speed and direction.
 */
void DataLogger::log(CarDataRecord& record) {
    this->file.write((const uint8_t*)&record, sizeof(record));
}

void DataLogger::flush() {
   this->file.flush();
   last_flush_time = millis();
}

void DataLogger::writeHeader() {
    file.print(F("micros"));
    file.println();
}
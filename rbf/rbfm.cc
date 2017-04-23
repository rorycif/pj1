#include "rbfm.h"
#include "pfm.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

RC RecordBasedFileManager::createFile(const string &fileName) {
    PagedFileManager * temp = PagedFileManager::instance();                 //access the methods
    if (!temp->createFile(fileName)) {                                      //return createfile's value, successfully created the file
        fileStorage[fileName] = isClosed;
        return 0;
    }
    return -1;
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
    PagedFileManager * temp = PagedFileManager::instance();
    return temp->destroyFile(fileName);
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
    PagedFileManager * temp = PagedFileManager::instance();
    fileStorageItr = fileStorage.find(fileName);
    if (fileStorageItr == fileStorage.end()) {
        //cout<< "file doesn't exist\n";
        return -1;
    }
    fileStorage[fileName] = isOpen;
    return temp->openFile(fileName, fileHandle);
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
    PagedFileManager * temp = PagedFileManager::instance();
    if (fileStorage[fileHandle.targetName] == isClosed) {
        return -1;
    }
            fileStorage[fileHandle.targetName] = isClosed;
            return temp->closeFile(fileHandle);
}

RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid) {
    float attributeTotal = recordDescriptor.size();      //number of bytes null flags take
    int nullbytes = ceil(attributeTotal / 8);            //the amount of bytes holding flags
    bool nullflag[nullbytes*8];                              //additional flags that check for nulls
    int size = nullbytes;
    if (!nullbytes)                                      
        return -1;                                       //no records exist
    char * curentbyte = (char*)data;                   
    for (int i = 0;i < nullbytes; i++){
        if(curentbyte[i] & 0b10000000){                    //first bit
            nullflag[0+(i*8)] = true;
//            cout<< (0+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b01000000){                    //second bit
            nullflag[1+(i*8)] = true;
//            cout<< (1+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00100000){                    //third bit
            nullflag[2+(i*8)] = true;
//            cout<< (2+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00010000){                    //fourth bit
            nullflag[3+(i*8)] = true;
//            cout<< (3+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00001000){                    //fifth bit
            nullflag[4+(i*8)] = true;
//            cout<< (4+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000100){                    //sixth bit
            nullflag[5+(i*8)] = true;
//            cout<< (5+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000010){                    //seventh bit
            nullflag[6+(i*8)] = true;
//            cout<< (6+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000001){                    //eigth bit
            nullflag[7+(i*8)] = true;
//            cout<< (7+(i*8))<<" is null\n";
        }
        data += sizeof(char);               //the flag byte has been used up, set pointer to next place                                   
    }
    int * tempInt;                          //used for casting to int
    float * tempFloat;                      //casts to float
    char * tempChar;                        //cast to chars
    int lengthof = 0;                       //running count of varchar length
    string charout = "";                    //output string
    for (int i = 0; i < recordDescriptor.size(); i++){
        if(!nullflag[i]){                       //attribute is not null
            switch(recordDescriptor[i].type){
                case(TypeInt):
                        tempInt = (int *)data;                                    //cast to int
//                        cout<<"    "<< recordDescriptor[i].name<< ": "<< tempInt[0];
                        data += sizeof(int);                                    //move on to next
                        size += sizeof(int);
                        break;
                case(TypeReal):
                        tempFloat = (float *)data;                                //cast to float
//                        cout<<"    "<< recordDescriptor[i].name<< ": "<< tempFloat[0];
                        data += sizeof(float);                                  //move on to next
                        size += sizeof(float);
                        break;
                case(TypeVarChar):
                        tempInt = (int *)data;                              //cast the size of varchar
                        data += sizeof(int);
                        tempChar = (char *)data;
                        for (unsigned j = 0 ; j < tempInt[0]; j++){     //concatiate string
                            charout += tempChar[j];
                            //cout << "varchar length: "<<j<< endl;
                            lengthof++;
                        }
//                        cout<<"    "<< recordDescriptor[i].name<< ": "<< charout;
                        data += lengthof;                                           //move that many bytes
                        size += lengthof;
                        lengthof = 0;                                               //reset counter
                        charout = "";                                           //output reset
                        break;
            }
        }
        else{                               
//            cout<< "    "<<recordDescriptor[i].name<<": NULL";
        }
    }
    cout<<"total bytes "<<size<<endl;
    
    return 0;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
    //the pgae of the record does not exist
    if (fileHandle.masterDirectory[rid.pageNum] == 0) {
      return -1;
    }
    SlotDirectory* pageDirectory = fileHandle.masterDirectory[rid.pageNum];
    //the record (slot number) does not exist in the page
    if (pageDirectory->findRecord(rid, data)) {
      return 0;
    }
    return -1;
}

RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {
    float attributeTotal = recordDescriptor.size();      //number of bytes null flags take
    int nullbytes = ceil(attributeTotal / 8);            //the amount of bytes holding flags
    bool nullflag[nullbytes*8];                              //additional flags that check for nulls
    if (!nullbytes)                                      
        return -1;                                       //no records exist
    char * curentbyte = (char*)data;                   
    for (int i = 0;i < nullbytes; i++){
        if(curentbyte[i] & 0b10000000){                    //first bit
            nullflag[0+(i*8)] = true;
//            cout<< (0+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b01000000){                    //second bit
            nullflag[1+(i*8)] = true;
//            cout<< (1+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00100000){                    //third bit
            nullflag[2+(i*8)] = true;
//            cout<< (2+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00010000){                    //fourth bit
            nullflag[3+(i*8)] = true;
//            cout<< (3+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00001000){                    //fifth bit
            nullflag[4+(i*8)] = true;
//            cout<< (4+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000100){                    //sixth bit
            nullflag[5+(i*8)] = true;
//            cout<< (5+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000010){                    //seventh bit
            nullflag[6+(i*8)] = true;
//            cout<< (6+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000001){                    //eigth bit
            nullflag[7+(i*8)] = true;
//            cout<< (7+(i*8))<<" is null\n";
        }
        data += sizeof(char);               //the flag byte has been used up, set pointer to next place                                   
    }
    int * tempInt;                          //used for casting to int
    float * tempFloat;                      //casts to float
    char * tempChar;                        //cast to chars
    int lengthof = 0;                       //running count of varchar length
    string charout = "";                    //output string
    for (int i = 0; i < recordDescriptor.size(); i++){
        if(!nullflag[i]){                       //attribute is not null
            switch(recordDescriptor[i].type){
                case(TypeInt):
                        tempInt = (int *)data;                                    //cast to int
                        cout<<"    "<< recordDescriptor[i].name<< ": "<< tempInt[0];
                        data += sizeof(int);                                    //move on to next
                        break;
                case(TypeReal):
                        tempFloat = (float *)data;                                //cast to float
                        cout<<"    "<< recordDescriptor[i].name<< ": "<< tempFloat[0];
                        data += sizeof(float);                                  //move on to next
                        break;
                case(TypeVarChar):
                        tempInt = (int *)data;                              //cast the size of varchar
                        data += sizeof(int);
                        tempChar = (char *)data;
                        for (unsigned j = 0 ; j < tempInt[0]; j++){     //concatiate string
                            charout += tempChar[j];
                            //cout << "varchar length: "<<j<< endl;
                            lengthof++;
                        }
                        cout<<"    "<< recordDescriptor[i].name<< ": "<< charout;
                        data += lengthof;                                           //move that many bytes
                        lengthof = 0;                                               //reset counter
                        charout = "";                                           //output reset
                        break;
            }
        }
        else{                               
            cout<< "    "<<recordDescriptor[i].name<<": NULL";
        }
    }
    cout<< "\n";
    return 0;
}

SlotDirectory::SlotDirectory(unsigned pageNum){
    this->pageNum = pageNum;
    this->numOfRecord = 0;
    this->freespace = PAGE_SIZE;
}

SlotDirectory::~SlotDirectory(){
}

RC SlotDirectory::addRecord(RID rid, void * data, unsigned sizeOfData) {
    //check if the record belong to this page
    if (rid.pageNum != this->pageNum) {
        return -1;
    }
    //check if the page has enough space
    if (sizeOfData > this->freespace) {
        return -1;
    }

    this->RIDs[rid.pageNum] = data;
    this->numOfRecord+=1;
    this->freespace-=sizeOfData;
    return 0;
}

RC SlotDirectory::findRecord(RID rid, void * data) {
    //check if the record is in this page
    if (rid.pageNum != this->pageNum) {
        return -1;
    }
    //no recrod in the page
    if (this->numOfRecord == 0) {
        return -1;
    }
    //return the address to the data if it is found
    if (this->RIDs[rid.slotNum]) {
        data = this->RIDs[rid.slotNum];
        return 0;
    } else {
        return -1;
    }
}

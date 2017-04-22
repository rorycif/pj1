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
/*    double size = ceil(recordDescriptor.size()/8);                          //finds the size 
    int n = (int)size;
    bitset<n> flags;
    for(int i =0; i < n; i++){
        if (data[i]){
                                                       //null flag
        }
        else{                                           //non null entry

        }
    }*/
    return -1;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
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
            cout<< (0+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b01000000){                    //second bit
            nullflag[1+(i*8)] = true;
            cout<< (1+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00100000){                    //third bit
            nullflag[2+(i*8)] = true;
            cout<< (2+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00010000){                    //fourth bit
            nullflag[3+(i*8)] = true;
            cout<< (3+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00001000){                    //fifth bit
            nullflag[4+(i*8)] = true;
            cout<< (4+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000100){                    //sixth bit
            nullflag[5+(i*8)] = true;
            cout<< (5+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000010){                    //seventh bit
            nullflag[6+(i*8)] = true;
            cout<< (6+(i*8))<<" is null\n";
        }
        if(curentbyte[i] & 0b00000001){                    //eigth bit
            nullflag[7+(i*8)] = true;
            cout<< (7+(i*8))<<" is null\n";
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
                        tempChar = (char *)data;
                        for (unsigned j = 0 ; j < recordDescriptor[i].length; j++){     //concatiate string
                            charout += tempChar[j];
                            cout << "varchar length: "<<j<< endl;
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
//    char t = curentbyte[0];
//    bitset<8> rawbits;
//    memcpy(&rawbits,&t,8);
//    cout<< rawbits<<endl;





/*        int totalBytes = 0;                                 //total bytes in record
//    cout << attributeTotal<< " attributes with " << nullbytes<< " byte to check \n";
    bitset<16> nullflags;                                //can hold up to 16 null flags
//    unsigned char test = 0b1101;
    memcpy(&nullflags,data,16);
    for (int i = 0; i < attributeTotal ; i++){
        if (nullflags[i])
            cout<< nullflags[i]<< " attribute "<< (attributeTotal-i) << " is null\n";
 //           recordDescriptor[attributeTotal-i-1].isNull = true;                         //set null flag for attribute
        else{
            //cout<< nullflags[i]<< " attibute "<< (attributeTotal-i)<< " is not null\n";     //increments the total byte count
            switch(recordDescriptor[attributeTotal-i-1].type){
                case TypeInt: totalBytes += sizeof(int);
                    cout<<recordDescriptor[attributeTotal-i-1].name<< " new byte count "<< totalBytes<< endl;
                    break;
                case TypeReal: totalBytes += sizeof(float);
                    cout<<recordDescriptor[attributeTotal-i-1].name<< " new byte count "<< totalBytes<< endl;
                    break;
                case TypeVarChar: totalBytes += recordDescriptor[attributeTotal-i].length;
                    cout<<recordDescriptor[attributeTotal-i-1].name<< " new byte count "<< totalBytes<< endl;
                    break;
            }
        }
    }
    cout<< "total bytes = "<< (totalBytes + nullbytes)<< endl;
    bitset<512> rawbits;                                        //take memory from data
    memcpy(&rawbits,data,totalBytes+nullbytes);                 //allocate
//    cout<< "stream "<< rawbits<< endl;
    rawbits >>= (nullbytes*8);                                  //move over null flags
//    cout<< "stream "<< rawbits<< endl;
    for (unsigned i =0; i< recordDescriptor.size(); i++){
//        cout << "attribute "<< i << " "<< recordDescriptor[i].name << " " << recordDescriptor[i].type << " "<< recordDescriptor[i].length << endl;
    }
//    while(1){

  //  }*/

    return 0;
}

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
	it = rbfms.find(fileName);
	if (it == rbfms.end()){              //file not found
	   rbfms[fileName] = fopen(fileName.c_str(), "w");                //creates blank file
	   fclose(rbfms[fileName]);                    //closes file after create
	   return 0;
    }
    else{
        return -1;
    }
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
    it = rbfms.find(fileName);
    if (it == rbfms.end()){              //file not found
	   return -1;
    }
    remove(fileName.c_str());       //delete file
    rbfms.erase(fileName);           //remove from map
    return 0;
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
    it = rbfms.find(fileName);
    if (it == rbfms.end()){
        return -1;                      //file does not exist
    }
    if (fileHandle.targetName == ""){    	//nothing is open
        rbfms[fileName] = fopen(fileName.c_str(), "r+");
        fileHandle.isOpen = true;
        fileHandle.targetName = fileName;
//        cout<< "opened: "<< fileHandle.targetName<<endl;
        return 0;
    }
    else{
//        cout<< "fileHandle is already open to "<< fileHandle.targetName<<endl;
        return -1;
    }
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
    it = rbfms.find(fileHandle.targetName);
    if (it == rbfms.end()){
        return -1;                      			//file does not exist
    }
    fflush(rbfms[fileHandle.targetName]);            //flushes everything changed to file
    fclose(rbfms[fileHandle.targetName]);
    fileHandle.targetName = "";                     //empty refrence
    fileHandle.isOpen = false;
//    cout<< "closed record file\n";                      //handle has closed file
    return 0;
}

RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid) {
    float attributeTotal = recordDescriptor.size();      //number of bytes null flags take
    char * tempdata = (char *)data;                      //to iterate without lossing original pointer
    unsigned nullbytes = ceil(attributeTotal / 8);            //the amount of bytes holding flags
    vector <bool> nullflag;                              //additional flags that check for nulls
    unsigned size = (unsigned)nullbytes;
    if (!nullbytes){                                      
        return -1;                                       //no records exist
    }
    char * currentbyte = (char*)tempdata;
//    cout<< "total null bytes = "<<nullbytes<<endl;                 
    for (unsigned i = 0;i < nullbytes; i++){
//        cout<< "checking nullbyte "<< i<<endl;
        if(currentbyte[i] & 0b10000000){                    //first bit
            nullflag.push_back(true);
//            cout<< (0+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b01000000){                    //second bit
            nullflag.push_back(true);
//            cout<< (1+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b00100000){                    //third bit
            nullflag.push_back(true);
//            cout<< (2+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b00010000){                    //fourth bit
            nullflag.push_back(true);
//            cout<< (3+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b00001000){                    //fifth bit
            nullflag.push_back(true);
//            cout<< (4+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b00000100){                    //sixth bit
            nullflag.push_back(true);
//            cout<< (5+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b00000010){                    //seventh bit
            nullflag.push_back(true);
//            cout<< (6+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        if(currentbyte[i] & 0b00000001){                    //eigth bit
            nullflag.push_back(true);
//            cout<< (7+(i*8))<<" is null\n";
        }
        else{
            nullflag.push_back(false);
        }
        tempdata += sizeof(char);               //the flag byte has been used up, set pointer to next place
        size += sizeof(char);                                   
    }
    int * tempInt;                          //used for casting to int
    char * tempChar;                        //cast to chars
    int lengthof = 0;                       //running count of varchar length
    string charout = "";                    //output string
//    cout<< "done checking nulls\n";
//    cout<<"for "<<size<< " to "<< recordDescriptor.size()<<"should equal "<<nullflag.size()<<endl;
    for (unsigned i = 0; i < recordDescriptor.size(); i++){
//        cout<< recordDescriptor[i].name<<" is";
        if(!nullflag[i]){                       //attribute is not null
//            cout<< " not null\n";
            switch(recordDescriptor[i].type){
                case(TypeInt):
                        tempInt = (int *)tempdata;                                    //cast to int
                        tempdata += sizeof(int);                                    //move on to next
                        size += sizeof(int);
                        break;
                case(TypeReal):
                        tempdata += sizeof(float);                                  //move on to next
                        size += sizeof(float);
                        break;
                case(TypeVarChar):
                        tempInt = (int *)tempdata;                              //cast the size of varchar
                        tempdata += sizeof(int);
                        size += sizeof(int);
                        tempChar = (char *)tempdata;
                        for (unsigned j = 0 ; j < (unsigned)tempInt[0]; j++){     //concatiate string
                            charout += tempChar[j];
                            lengthof++;
                        }
                        tempdata += lengthof;                                           //move that many bytes
                        size += lengthof;
                        lengthof = 0;                                               //reset counter
                        charout = "";                                           //output reset
                        break;
            }

        }
        else{                               
//            cout<< " null\n";
        }
    }
    int available = fileHandle.getAvailablePage(size);
    int prevOff = fileHandle.offset[available];
//    cout<<"total bytes "<<size<<" will be inserted into "<< available<< endl;
    fseek(rbfms[fileHandle.targetName],(PAGE_SIZE * available) + prevOff,SEEK_SET);
    fwrite(data,size,1,rbfms[fileHandle.targetName]);
    fflush(rbfms[fileHandle.targetName]);
    fileHandle.offset[available] = prevOff + size;                   //increments where free space begins
    fileHandle.masterDirectory[available]->numOfRecord++;
    rid.pageNum = available;                                            //update rid values
    rid.slotNum = fileHandle.masterDirectory[available]->numOfRecord;
    fileHandle.masterDirectory[available]->individualOff.push_back(size);
//    cout<< "file "<<fileHandle.targetName << " now has this many records "<< fileHandle.masterDirectory[available]->numOfRecord<< " new offset on page "<< available <<" is "<< fileHandle.offset[available]<< endl;
//    cout<< "inserted into slot, page "<< rid.slotNum << " "<< rid.pageNum<<endl;
    return 0;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
   //the page or slot does not exist
//   cout<< "slot and page "<< rid.slotNum<< " "<<rid.pageNum<<endl;
   if (rid.pageNum > fileHandle.getNumberOfPages() || rid.slotNum > fileHandle.masterDirectory[rid.pageNum]->numOfRecord) {
     return -1;
   }
   //sum individual offsets
   int sum = 0;                                     //offset within page
   for (unsigned i = 0 ; i<rid.slotNum-1; i++){
        sum += fileHandle.masterDirectory[rid.pageNum]->individualOff[i];
   }
   int offset = (rid.pageNum * PAGE_SIZE) + sum;    //from beigning of file
   int end = offset + fileHandle.masterDirectory[rid.pageNum]->individualOff[rid.slotNum-1];
//   cout<<"slot offset "<< offset<< " end in "<< end<<endl;
   fseek(rbfms[fileHandle.targetName],offset,SEEK_SET);
   fread(data,end,1,rbfms[fileHandle.targetName]);
//   cout<< sum<<" here\n";
   //read from page
//   cout<< "number of attributes: "<< recordDescriptor.size()<<endl;
   for (unsigned i = 0; i < recordDescriptor.size(); i++){
//        cout<<i<< " "<< recordDescriptor[i].name<<endl;
   }
//   cout<< "read record\n";
//    printRecord(recordDescriptor, data);
   return 0;

}

RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {
    float attributeTotal = recordDescriptor.size();      //number of bytes null flags take
    int size =0;
    char * tempdata = (char*)data;
    int nullbytes = ceil(attributeTotal / 8);            //the amount of bytes holding flags
    bool nullflag[nullbytes*8];                              //additional flags that check for nulls
    if (!nullbytes)                                      
        return -1;                                       //no records exist
    char * curentbyte = tempdata;                   
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
        tempdata += sizeof(char);               //the flag byte has been used up, set pointer to next place
        size += sizeof(char);
        cout<< "size of nulls"<< size<<endl;                                   
    }
    int * tempInt;                          //used for casting to int
    float * tempFloat;                      //casts to float
    char * tempChar;                        //cast to chars
    int lengthof = 0;                       //running count of varchar length
    string charout = "";                    //output string
    for (unsigned i = 0; i < recordDescriptor.size(); i++){
        if(!nullflag[i]){                       //attribute is not null
            switch(recordDescriptor[i].type){
                case(TypeInt):
                        tempInt = (int *)tempdata;                                    //cast to int
                        cout<<"    "<< recordDescriptor[i].name<< ": "<< tempInt[0];
                        tempdata += sizeof(int);                                    //move on to next
                        size += sizeof(int);
                        break;
                case(TypeReal):
                        tempFloat = (float *)tempdata;                                //cast to float
                        cout<<"    "<< recordDescriptor[i].name<< ": "<< tempFloat[0];
                        tempdata += sizeof(float);                                  //move on to next
                        size += sizeof(float);
                        break;
                case(TypeVarChar):
                        tempInt = (int *)tempdata;                              //cast the size of varchar
                        tempdata += sizeof(int);
                        size += sizeof(int);
                        tempChar = (char *)tempdata;
                        for (unsigned j = 0 ; j < (unsigned)tempInt[0]; j++){     //concatiate string
                            charout += tempChar[j];
                            //cout << "varchar length: "<<j<< endl;
                            lengthof++;
                        }
                        cout<<"    "<< recordDescriptor[i].name<< ": "<< charout;
                        tempdata += lengthof;                                           //move that many bytes
                        size += lengthof;
                        lengthof = 0;                                               //reset counter
                        charout = "";                                           //output reset
                        break;
            }
        }
        else{                               
            cout<< "    "<<recordDescriptor[i].name<<": NULL";
        }
    }
//    cout<< "\n printsize "<< size<<endl;
    cout<< "\n";
    return 0;
}

SlotDirectory::SlotDirectory(unsigned Num){
    pageNum = Num;
    numOfRecord = 0;
    freespace = PAGE_SIZE;
}

SlotDirectory::~SlotDirectory(){
}
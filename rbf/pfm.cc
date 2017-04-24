#include "pfm.h"
#include "rbfm.h"

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
}


RC PagedFileManager::createFile(const string &fileName)
{
	it = pfms.find(fileName);
	if (it == pfms.end()){              //file not found

	   pfms[fileName] = fopen(fileName.c_str(), "w");                //creates blank file
	   fclose(pfms[fileName]);                    //closes file after create
	   return 0;
	}
	else{                               //file already exist*/
	   return -1;
	}
}


RC PagedFileManager::destroyFile(const string &fileName)
{
    it = pfms.find(fileName);
    if (it == pfms.end()){
        return -1;                      //file not found
    }
    else{
        remove(fileName.c_str());       //delete file
        pfms.erase(fileName);           //remove from map
        return 0;
    }
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    it = pfms.find(fileName);
    if (it == pfms.end()){
        return -1;                      //file does not exist
    }
    if (fileHandle.targetName == ""){    	//nothing is open
        pfms[fileName] = fopen(fileName.c_str(), "r+");
        fileHandle.isOpen = true;
        fileHandle.targetName = fileName;
        fileHandle.pfmPointer = this;
        return 0;
    }
    else{
        cout<< "fileHandle is already open to "<< fileHandle.targetName<<endl;
        return -1;
    }
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    it = pfms.find(fileHandle.targetName);
    if (it == pfms.end()){
        return -1;                      			//file does not exist
    }

    fflush(pfms[fileHandle.targetName]);            //flushes everything changed to file

    fclose(pfms[fileHandle.targetName]);
    fileHandle.targetName = "";                     //empty refrence
    fileHandle.isOpen = false;                      //handle has closed file
    return 0;
}


FileHandle::FileHandle()
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
}


FileHandle::~FileHandle()
{
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
	if (appendPageCounter < pageNum){
		return -1;					//the page must not exist
	}
    int offSet = pageNum * PAGE_SIZE;             //this is where the page begins
    fseek(pfmPointer->pfms[targetName],offSet, SEEK_SET);     //goes to the page to be read
    fread(data,PAGE_SIZE,1,pfmPointer->pfms[targetName]);     //reads page to data pointer
//    cout<< "read from page: "<< (char*)data<<endl;
    readPageCounter++;                              //increments counter
    return 0;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
	if (appendPageCounter < pageNum){
		return -1;					//page must not exist
	}
//    cout<<"has this many pages: "<< getNumberOfPages()<< endl;
//    cout<< "data that will be wrtten to page "<< pageNum<< ": " << (char*)data << endl;
    int offset = pageNum * PAGE_SIZE;                     //where the page begins
    fseek(pfmPointer->pfms[targetName],offset, SEEK_SET);       //find page start
    fwrite(data, PAGE_SIZE,1, pfmPointer->pfms[targetName]);    //write to memory
    fflush(pfmPointer->pfms[targetName]);                       //commit to disk
    writePageCounter++;                             //increments counter
    return 0;
}


RC FileHandle::appendPage(const void *data)
{
//    cout<< "appending to this page count: " << getNumberOfPages()<< endl;
    fseek(pfmPointer->pfms[targetName],0, SEEK_END);                //find end of file
    fwrite(data, PAGE_SIZE, 1, pfmPointer->pfms[targetName]);       //write to memory
    fflush(pfmPointer->pfms[targetName]);                          //commit to disk
    SlotDirectory * temp =  new SlotDirectory(getNumberOfPages());
    masterDirectory.push_back(temp);                     //add slot directory to page
    offset.push_back(0);   //add what the offest is                //for slot based directories this is the offset
    cout << offset[getNumberOfPages()-1]<< " offset\n";
    appendPageCounter++;                                            //increments counter
//    cout<< "now has this many pages: "<< getNumberOfPages()<<endl;
    return 0;
}


unsigned FileHandle::getNumberOfPages()
{
    FILE *p_file = NULL;
    p_file = fopen(targetName.c_str(), "rb");
    fseek(p_file,0,SEEK_END);                                       //get to the last byte
    int size = ftell(p_file);
    fclose(p_file);
    return (size / PAGE_SIZE);                                      //return the page count
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    readPageCounter = readPageCount;
    writePageCounter = writePageCount;
    appendPageCounter = writePageCount;
    return 0;
}

int FileHandle::getAvailablePage(unsigned size){
    if(size > PAGE_SIZE){
        return -1;                  //record is too big
    }
    if(masterDirectory.empty()){
        SlotDirectory * temp =  new SlotDirectory(getNumberOfPages());
        masterDirectory.push_back(temp);
        offset.push_back(0);   //add what the offest is
        return 0;
    }
    for (unsigned i = 0; i < masterDirectory.size(); i++){    //itate through all slot directories
//        cout << "In page " << getNumberOfPages() << ", it has freespace " << masterDirectory[i]->freespace << endl;
        if(PAGE_SIZE - offset[i] >= size){
            return masterDirectory[i]->pageNum;        //there exist a page to add to
        }
    }
    SlotDirectory * temp =  new SlotDirectory(getNumberOfPages());
    masterDirectory.push_back(temp);
    offset.push_back(0);   //add what the offest is
    return masterDirectory.size()-1;        //there was no available page so we make another
    
}

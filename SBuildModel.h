/*
* SBuildModel.h  SDHumming Model Building routines
*
* Copyright (c) 2010 Shanda Interactive Entertainment Limited.
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or   
* any later version. See the COPYRIGHT.txt for more details.       
*/

#ifndef SDHUMMING_SBUILDMODEL_H_
#define SDHUMMING_SBUILDMODEL_H_

#include <vector>
#include <map>
#include <string>
using namespace std;

typedef struct {
	int iNoteStartTime;
	int iDurationms;
	int iDurationWithRest;
	int iNote;
	int iPhraseTag;
	int itrackNo;
}NoteStructure;

typedef struct {
	string MidiFilename;
	string MetaInfo;
}MidiMetaInfo;

typedef struct{
	float Pitch;  
	float Duration;
	float DurationWithRest;
}NOTE_TEMPLATE;

typedef std::vector<NOTE_TEMPLATE> MELODY_TEMPLATE;

typedef struct{
	MELODY_TEMPLATE notes;
	int id;
	string MetaInfo;
	vector<int> PhraseOffsetVector;
} ModelStru;

typedef struct{
	int Startms;
	int Durationms;
	int Channel;
	int type;
	int velocity;
}MIDIINFO;

typedef std::vector<ModelStru> IntQBHModelMap;

/*
 * note: 我们要做的是在 它那个 list.txt 文件里只存一首歌，然后把输出改成 对应的歌名.model 和 对应的歌名.info (原先是QBHModel.model 和 QBHModel.info 节操啊...)
 *       只要在另一个程序里指定要调用的 model 和 info 就可以了
 * .note end
 */
class BuildSDHummingModel{
public:
	BuildSDHummingModel(char *pszFileList, char *pszModelDir){
		TotalNoteNumber=0;
		strcpy(m_pszFileList,pszFileList);
		strcpy(m_pszModelDir,pszModelDir);
	}
	
	~BuildSDHummingModel(){
	}
	
	bool ExtractTrackNo(int TrackNo,string MetaInfo,char* filename);
	int writeExtractedTrack(char *OutputModelFileName);
	int writeSongInfo(char *OutputModelFileName);
	int GenFilelist();
	
	int Write2Model();
	vector<MIDIINFO> LoadMidiFile(char* szInFile);
	int ReadFromNotes(vector<MIDIINFO> myNotes);
	string GetSongName(string myStr);
	
private:
	NoteStructure *pMidiNoteStruct; 
	vector<NoteStructure> m_pExtractedTrack;
	int TotalNoteNumber; 

	char OutputModelFileName[1024]; 
	
	multimap<int, MidiMetaInfo> m_FileTrackmap;
	
	IntQBHModelMap m_QBHModel;
	
	char m_pszFileList[1024];
	char m_pszModelDir[1024];
	
	vector<string> m_SongNameVector;
};

#endif
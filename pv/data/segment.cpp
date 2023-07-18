/**
 ****************************************************************************************************
 * @author      正点原子团队(ALIENTEK)
 * @date        2023-07-18
 * @license     Copyright (c) 2023-2035, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:zhengdianyuanzi.tmall.com
 *
 ****************************************************************************************************
 */

#include "segment.h"
#include "qdebug.h"

const quint64 Segment::LevelMask[2] = {
    (quint64)~(~0ULL << 44) << 20,
    (quint64)~(~0ULL << 38) << 26
};
const quint64 Segment::LevelOffset[2] = {
    (quint64)~(~0ULL << 20),
    (quint64)~(~0ULL << 26)
};

Segment::Segment() :Segment(0)
{}

Segment::Segment(quint8 channelNum)
{
    for (int i = 0; i < channelNum; i++)
        AddChannel();
}

Segment::~Segment()
{
    for (int i = 0; i < _m_DataChunks.size(); i++) {
        for (int ii = 0; ii < _m_DataChunks[i].size(); ii++) {
            if (_m_DataChunks[i][ii].lbp != nullptr)
            {
                for (size_t j = 0; j < m_NODE_DATA_SIZE; j++) {
                    if (_m_DataChunks[i][ii].lbp[j])
                        free(_m_DataChunks[i][ii].lbp[j]);
                }
            }
        }
        _m_DataChunks[i].clear();
    }
    _m_DataChunks.clear();
}

quint8 Segment::GetChannelNum()
{
    return _m_ChannelNum;
}

void Segment::AddChannel()
{
    QVector<_Node> root_vector;
    for (size_t j = 0; j < _m_CHUNK_DATA_SIZE; j++) {
        struct _Node nd;
        nd.tog = 0;
        nd.value = 0;
        for(size_t i = 0; i < m_NODE_DATA_SIZE; i++){
            nd.lbpNode[i].tog=0;
            nd.lbpNode[i].value=0;
        }

        memset(nd.lbp, 0, sizeof(nd.lbp));
        root_vector.push_back(nd);

    }
    _m_DataChunks.push_back(root_vector);
    isData.push_back(false);
    m_isFirst.push_back(true);
    m_maxSampling.push_back(0);
    _m_startOffsetSampling.push_back(0);
    _m_ChannelGlitchRemoval.push_back(0);
    _m_ChannelNum++;
}

void Segment::SetChannelGlitchRemoval(quint8 ucChannelId, quint32 ulNum)
{
    if(!checkChannelId(ucChannelId))
        return;
    _m_ChannelGlitchRemoval[ucChannelId]=ulNum;
}

quint16 Segment::GetChannelGlitchRemoval(quint8 ucChannelId)
{
    if(!checkChannelId(ucChannelId))
        return 0;
    return _m_ChannelGlitchRemoval[ucChannelId];
}

SEGMENT_MSG Segment::SetSampleBlock(quint8 ucChannelId, quint64 ullStartPosition, quint8* data, quint32 len, bool isCheckBlockCompress)
{
    if(!checkChannelId(ucChannelId))
        return ATK_CHANNEL_ERROR;

    quint16 ullNodeIndex = (ullStartPosition & LevelMask[1]) >> 26;			
    quint32 ullNodeOffset = ullStartPosition & LevelOffset[1];				
    quint32 ullBlockIndex = (ullNodeOffset & LevelMask[0]) >> 20;	
    quint32 ullBlockOffset = (ullNodeOffset & LevelOffset[0]);		

    if(ullNodeIndex>=_m_CHUNK_DATA_SIZE)
        return ATK_POSITION_ERROR;
    
    quint32 count=0;
    while(len>0){
        quint32 writeLen=qMin(m_BLOCK_MAX_SIZE-ullBlockOffset,len);
        bool isCheckCompress=ullBlockOffset+len>=m_BLOCK_MAX_SIZE;
        
        if (_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex] == NULL)
        {
            _m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex] = malloc(m_BLOCK_MAX_SIZE);
            if (_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex] == NULL){
                m_isMemoryError=true;
                LogHelp::write("SetSampleBlock 申请内存失败。");
                return ATK_MEMORY_ERROR;
            }
            memset(_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex], 0, m_BLOCK_MAX_SIZE);
        }
        _m_dataMutex.lock();
        void* dst=(quint8*)_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex]+ullBlockOffset;
        memcpy(dst,data,writeLen);
        len-=writeLen;
        data+=writeLen;
        count+=writeLen;
        if(isCheckCompress&&isCheckBlockCompress)
            CheckBlockCompress(ucChannelId,ullNodeIndex,ullBlockIndex);
        _m_dataMutex.unlock();
        ullBlockIndex++;
        ullBlockOffset=0;
        if (ullBlockIndex >= m_NODE_DATA_SIZE) {
            ullBlockIndex = 0;
            ullNodeIndex++;
        }
    }
    if(m_maxSampling[ucChannelId]<(ullStartPosition+count)*8)
        m_maxSampling[ucChannelId]=(ullStartPosition+count)*8;
    isData[ucChannelId]=true;
    return ATK_CORRECT;
}

void Segment::CheckBlockCompress(quint8 ucChannelId, quint16 ullNodeIndex, quint32 ullBlockIndex)
{
    quint8* lbp=(quint8*)_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex];
    if(lbp!=nullptr){
        bool current=*lbp;
        if(CheckCompress(current,lbp,m_BLOCK_MAX_SIZE)){
            _m_DataChunks[ucChannelId][ullNodeIndex].tog |= 1ULL << ullBlockIndex;
            if(current)
                _m_DataChunks[ucChannelId][ullNodeIndex].value |= 1ULL << ullBlockIndex;
            else
                _m_DataChunks[ucChannelId][ullNodeIndex].value &= ~(1ULL << ullBlockIndex);
            free(_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex]);
            _m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex]=nullptr;
            _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].tog=~0ull;
            if(current)
                _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value=~0ull;
            else
                _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value=0;
        }else{
            
            for(qint32 i=0;i<64;i++){
                if(CheckCompress(current,lbp,m_BLOCK_NODE_SIZE)){
                    _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].tog |= 1ULL << i;

                    if(current)
                        _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value |= 1ULL << i;
                    else
                        _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value &= ~(1ULL << i);
                }
                lbp+=m_BLOCK_NODE_SIZE;
            }
        }
    }else{
        
        _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].tog=~0ull;
        if(_m_DataChunks[ucChannelId][ullNodeIndex].value & (1ULL << ullBlockIndex))
            _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value=~0ull;
        else
            _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value=0;
    }
}

bool Segment::CheckCompress(bool current,quint8 *lbp, quint64 len)
{
    quint64 current64Mask=current?~0ull:0;
    qint32 count=len/8;
    quint64* ptr=(quint64*)lbp;
    for(quint32 i=0;i<count;i++){
        if(current64Mask!=*ptr)
            return false;
        ptr++;
    }
    return true;
}

quint8* Segment::GetSampleBlock(quint64 ullStartSample, quint64& ullEndSample, quint8 ucChannelId)
{
    if(!checkChannelId(ucChannelId))
        return NULL;
    quint64 ullBit = ullStartSample >> 3;							
    quint64 ullNodeIndex = (ullBit & LevelMask[1]) >> 26;			
    quint32 ullNodeOffset = ullBit & LevelOffset[1];				
    quint64 ullBlockIndex = (ullNodeOffset & LevelMask[0]) >> 20;	
    ullEndSample = ((ullNodeIndex << 26) + ((ullBlockIndex + 1) << 20)) << 3;
    _m_dataMutex.lock();
    quint8* ref = (quint8*)_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex];
    _m_dataMutex.unlock();
    if (ref == NULL)
        return NULL;
    return ref + (ullNodeOffset & LevelOffset[0]);
}

bool Segment::GetSample(quint64 ullStartSample, quint8 ucChannelId, bool appendOffset)
{
    if(!checkChannelId(ucChannelId))
        return false;
    if(appendOffset)
        ullStartSample+=_m_startOffsetSampling[ucChannelId];
    quint64 ullBit = ullStartSample >> 3;							
    quint8 ullBitOffset = ullStartSample & 7;						
    quint64 ullNodeIndex = (ullBit & LevelMask[1]) >> 26;			
    quint32 ullNodeOffset = ullBit & LevelOffset[1];				
    quint64 ullBlockIndex = (ullNodeOffset & LevelMask[0]) >> 20;	
    quint32 ullBlockOffset = (ullNodeOffset & LevelOffset[0]);		

    if(ullNodeIndex>=_m_DataChunks[ucChannelId].length())
        return false;
    _m_dataMutex.lock();
    quint8* ref = (quint8*)_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex];
    _m_dataMutex.unlock();
    
    if ((_m_DataChunks[ucChannelId][ullNodeIndex].tog & 1ULL << ullBlockIndex) != 0)
        return _m_DataChunks[ucChannelId][ullNodeIndex].value & 1ULL << ullBlockIndex;
    if (ref == NULL)
        return false;
    return  *(ref + ullBlockOffset) & 1ULL << ullBitOffset;
}

void Segment::SetSampleBlock(quint64 ullStartSample, quint64 ullEndSample, bool isHigh, quint8 ucChannelId, bool appendOffset)
{
    if(!checkChannelId(ucChannelId))
        return;
    if(appendOffset){
        ullStartSample+=_m_startOffsetSampling[ucChannelId];
        ullEndSample+=_m_startOffsetSampling[ucChannelId];
    }
    quint64 ullBit = ullStartSample >> 3;							
    quint8 ullBitOffset = ullStartSample & 7;						
    quint64 ullNodeIndex = (ullBit & LevelMask[1]) >> 26;			
    quint32 ullNodeOffset = ullBit & LevelOffset[1];				
    quint64 ullBlockIndex = (ullNodeOffset & LevelMask[0]) >> 20;	
    quint32 ullBlockOffset = (ullNodeOffset & LevelOffset[0]);		
    quint32 ullLpdNodeIndex = ullBlockOffset>>14;                   

    _m_dataMutex.lock();
    quint8* lbp = (quint8*)_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex];
    _m_dataMutex.unlock();
    while(ullStartSample<ullEndSample){
        if ((_m_DataChunks[ucChannelId][ullNodeIndex].tog & 1ULL << ullBlockIndex) == 0){
            if(ullBitOffset!=0||ullEndSample-ullStartSample<8){
                while(true){
                    if(isHigh)
                        *(lbp + ullBlockOffset) |= 1 << ullBitOffset;
                    else
                        *(lbp + ullBlockOffset) &= ~(1 << ullBitOffset);
                    ullBitOffset++;
                    ullStartSample++;
                    if(ullBitOffset>=8||ullStartSample>=ullEndSample){
                        ullBitOffset=0;
                        break;
                    }
                }
            }else{
                if(isHigh)
                    *(lbp + ullBlockOffset)=255;
                else
                    *(lbp + ullBlockOffset)=0;
                ullStartSample+=8;
            }
            if((_m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].tog & 1ULL << ullLpdNodeIndex) != 0)
                _m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].tog &= ~(1ULL << ullLpdNodeIndex);
            ullBlockOffset++;
            ullLpdNodeIndex = ullBlockOffset>>14;
            if(ullBlockOffset>=m_BLOCK_MAX_SIZE){
                ullBlockOffset=0;
                ullBlockIndex++;
            }
        }else
            break;
    }
}

quint64 Segment::GetMaxSample(quint8 ucChannelId, bool appendOffset)
{
    if(!checkChannelId(ucChannelId))
        return 0;
    if(appendOffset){
        if(m_maxSampling[ucChannelId]<=_m_startOffsetSampling[ucChannelId])
            return 0;
        else
            return m_maxSampling[ucChannelId]-_m_startOffsetSampling[ucChannelId];
    }else
        return m_maxSampling[ucChannelId];
}

void Segment::SetMaxSample(quint64 ullMaxSample, quint8 ucChannelId, bool appendOffset)
{
    if(!checkChannelId(ucChannelId))
        return;
    if(appendOffset)
        m_maxSampling[ucChannelId]=ullMaxSample+_m_startOffsetSampling[ucChannelId];
    else
        m_maxSampling[ucChannelId]=ullMaxSample;
}

quint64 Segment::GetStartOffsetSampling(quint8 ucChannelId)
{
    if(ucChannelId>=_m_startOffsetSampling.count())
        return 0;
    return _m_startOffsetSampling[ucChannelId];
}

void Segment::SetStartOffsetSampling(quint8 ucChannelId, quint64 num)
{
    if(ucChannelId>=_m_startOffsetSampling.count())
        return;
    _m_startOffsetSampling[ucChannelId]=num;
}

DataEnd Segment::GetDataEnd(quint64 ullStartSample, quint8 ucChannelId, bool isBackward)
{
    DataEnd ret;
    if(!checkChannelId(ucChannelId))
        return ret;
    
    ullStartSample+=_m_startOffsetSampling[ucChannelId];

    quint64 ullBit = ullStartSample >> 3;							
    quint8 ullBitOffset = ullStartSample & 7;						
    quint64 ullNodeIndex = (ullBit & LevelMask[1]) >> 26;			
    quint32 ullNodeOffset = ullBit & LevelOffset[1];				
    quint64 ullBlockIndex = (ullNodeOffset & LevelMask[0]) >> 20;	
    quint32 ullBlockOffset = (ullNodeOffset & LevelOffset[0]);		
    quint32 ullLpdNodeIndex = ullBlockOffset>>14;                   

    if(ullNodeIndex>=_m_DataChunks[ucChannelId].length())
        return ret;

    bool current=GetSample(ullStartSample,ucChannelId,false);
    quint8 currentMask=current?255:0;
    quint64 current64Mask=current?~0ull:0;
    quint64 maxSample=GetMaxSample(ucChannelId,false)-1;
    quint64 maxSampleOffset=maxSample+_m_startOffsetSampling[ucChannelId];
    while(true){
        
        if ((_m_DataChunks[ucChannelId][ullNodeIndex].tog & 1ULL << ullBlockIndex) != 0) {
            if((bool)(_m_DataChunks[ucChannelId][ullNodeIndex].value & 1ULL << ullBlockIndex)!=current)
                goto end;
        }else{
            _m_dataMutex.lock();
            quint8* lbp = (quint8*)_m_DataChunks[ucChannelId][ullNodeIndex].lbp[ullBlockIndex];
            if(lbp==nullptr){
                _m_dataMutex.unlock();
                goto end;
            }
            if(isBackward){
                while(true){
                    if(*(lbp + ullBlockOffset)!=currentMask){
                        while(true){
                            if((bool)(*(lbp + ullBlockOffset) & 1 << ullBitOffset)!=current){
                                _m_dataMutex.unlock();
                                goto end;
                            }
                            ullBitOffset--;
                            if(ullBitOffset==255)
                                break;
                        }
                    }
                    ullBitOffset=7;
                    ullBlockOffset--;
                    if(ullBlockOffset>=m_BLOCK_MAX_SIZE)
                        break;
                }
            }else{
                while(true){
                    bool isSingle=true;
                    if((_m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].tog & 1ULL << ullLpdNodeIndex) != 0){

                        if((bool)(_m_DataChunks[ucChannelId][ullNodeIndex].lbpNode[ullBlockIndex].value & 1ULL << ullLpdNodeIndex)==current){
                            ullLpdNodeIndex++;
                            ullBlockOffset=ullLpdNodeIndex*m_BLOCK_NODE_SIZE;
                            isSingle=false;
                        }
                    }
                    if(isSingle){
                        if(ullBlockOffset+8<m_BLOCK_MAX_SIZE){
                            quint64 tmp=*((quint64*)(lbp + ullBlockOffset));
                            if(tmp==current64Mask){
                                ullBlockOffset+=8;
                                isSingle=false;
                            }
                        }
                    }
                    if(isSingle){
                        if(*(lbp + ullBlockOffset)!=currentMask){
                            while(true){
                                if((bool)(*(lbp + ullBlockOffset) & 1 << ullBitOffset)!=current){
                                    _m_dataMutex.unlock();
                                    goto end;
                                }
                                ullBitOffset++;
                                if(ullBitOffset>=8)
                                    break;
                            }
                        }
                        ullBlockOffset++;
                    }
                    ullBitOffset=0;
                    ullLpdNodeIndex = ullBlockOffset>>14;
                    if(ullBlockOffset>=m_BLOCK_MAX_SIZE)
                        break;
                }
            }
            _m_dataMutex.unlock();
        }
        if(isBackward){
            if(ullBlockIndex==0){
                ullBlockIndex=m_NODE_DATA_SIZE-1;
                if(ullNodeIndex==0)
                    return ret;
                ullNodeIndex--;
            }else
                ullBlockIndex--;
            ullBlockOffset=m_BLOCK_MAX_SIZE-1;
            ullBitOffset=7;
        }else{
            ullBlockIndex++;
            if(ullBlockIndex>=m_NODE_DATA_SIZE){
                ullBlockIndex=0;
                ullNodeIndex++;
                if(ullNodeIndex>=_m_DataChunks[ucChannelId].length()){
                    ret.position=maxSample;
                    ret.isEnd=true;
                    return ret;
                }

            }
            ullBlockOffset=0;
            ullBitOffset=0;
            if((ullNodeIndex<<26)+(ullBlockIndex<<20)>=maxSampleOffset){
                ret.position=maxSample;
                ret.isEnd=true;
                return ret;
            }
        }
        ullLpdNodeIndex = ullBlockOffset>>14;
    }
end:
    ret.position=(((ullNodeIndex<<26)+(ullBlockIndex<<20)+ullBlockOffset)<<3)+ullBitOffset+(isBackward?1:0);
    if(ret.position<=_m_startOffsetSampling[ucChannelId])
        ret.position=0;
    else
        ret.position-=_m_startOffsetSampling[ucChannelId];
    if(ret.position>maxSample){
        ret.position=maxSample;
        ret.isEnd=true;
    }
    return ret;
}

qint64 Segment::GetSamplingFrequency() {
    return _m_SamplingFrequency;
}

void Segment::SetSamplingFrequency(qint64 ullSamplingFrequency) {
    if (ullSamplingFrequency > 0)
    {
        _m_SamplingFrequency = ullSamplingFrequency;
        _m_Multiply=1000000/ullSamplingFrequency;
    }
}
quint32 Segment::GetMultiply()
{
    return _m_Multiply;
}

bool Segment::checkChannelId(quint8 ucChannelId)
{
    if(ucChannelId<0||ucChannelId>=_m_ChannelNum)
        return false;
    return true;
}

Segment *Segment::Clone()
{
    Segment* tmp=new Segment(_m_ChannelNum);
    tmp->m_SamplingDepth=m_SamplingDepth;
    tmp->m_TriggerSamplingDepth=m_TriggerSamplingDepth;
    tmp->SetSamplingFrequency(GetSamplingFrequency());
    for(qint32 i=0;i<_m_ChannelNum;i++){
        QVector<Segment::_Node>* dataList=tmp->GetChannelDataList(i);
        tmp->SetStartOffsetSampling(i,GetStartOffsetSampling(i));
        tmp->SetChannelGlitchRemoval(i,GetChannelGlitchRemoval(i));
        tmp->SetMaxSample(GetMaxSample(i,false),i,false);
        tmp->isData[i]=isData[i];
        for(qint32 ii=0;ii<dataList->count();ii++){
            (*dataList)[ii].tog=_m_DataChunks[i][ii].tog;
            (*dataList)[ii].value=_m_DataChunks[i][ii].value;
            for(quint32 j=0;j<m_NODE_DATA_SIZE;j++){
                if(_m_DataChunks[i][ii].lbp[j]){
                    (*dataList)[ii].lbp[j]=malloc(m_BLOCK_MAX_SIZE);
                    if ((*dataList)[ii].lbp[j] == NULL){
                        m_isMemoryError=true;
                        LogHelp::write("Clone 申请内存失败。");
                        continue;
                    }
                    memcpy((*dataList)[ii].lbp[j],_m_DataChunks[i][ii].lbp[j],m_BLOCK_MAX_SIZE);
                }
                (*dataList)[ii].lbpNode[j].tog=_m_DataChunks[i][ii].lbpNode[j].tog;
                (*dataList)[ii].lbpNode[j].value=_m_DataChunks[i][ii].lbpNode[j].value;
            }
        }
    }
    return tmp;
}

void Segment::appendCite()
{
    _m_citeMutex.lock();
    _m_cite++;
    _m_citeMutex.unlock();
}

void Segment::lessenCite()
{
    _m_citeMutex.lock();
    if(_m_cite>0)
        _m_cite--;
    _m_citeMutex.unlock();
}

bool Segment::isCite()
{
    bool ret;
    _m_citeMutex.lock();
    ret=_m_cite!=0;
    _m_citeMutex.unlock();
    return ret;
}

QVector<Segment::_Node> *Segment::GetChannelDataList(qint32 channelID)
{
    if(channelID<0||channelID>=_m_DataChunks.count())
        return nullptr;
    else
        return &_m_DataChunks[channelID];
}

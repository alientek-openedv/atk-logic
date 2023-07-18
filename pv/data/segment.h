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

#ifndef SEGMENT_H
#define SEGMENT_H

#pragma once

#include <QVector>
#include <QBitArray>
#include <QMutex>
#include "qmath.h"
#include "./pv/static/log_help.h"

using namespace std;

struct DataEnd
{
    quint64 position=0;
    bool isEnd=false;
};

enum SEGMENT_MSG{
    ATK_CORRECT,
    ATK_MEMORY_ERROR,
    ATK_CHANNEL_ERROR,
    ATK_POSITION_ERROR
};

class Segment
{

public:
    Segment();
    Segment(quint8 channelNum);
    ~Segment();
    quint8 GetChannelNum();
    void AddChannel();
    void SetChannelGlitchRemoval(quint8 ucChannelId, quint32 ulNum);
    quint16 GetChannelGlitchRemoval(quint8 ucChannelId);
    SEGMENT_MSG SetSampleBlock(quint8 ucChannelId, quint64 ullStartPosition, quint8* data, quint32 len, bool isCheckBlockCompress);
    quint8* GetSampleBlock(quint64 ullStartSample, quint64& ullEndSample, quint8 ucChannelId);
    bool GetSample(quint64 ullStartSample, quint8 ucChannelId, bool appendOffset=true);
    void SetSampleBlock(quint64 ullStartSample, quint64 ullEndSample, bool isHigh, quint8 ucChannelId, bool appendOffset);
    quint64 GetMaxSample(quint8 ucChannelId, bool appendOffset=true);
    void SetMaxSample(quint64 ullMaxSample,quint8 ucChannelId, bool appendOffset=true);
    quint64 GetStartOffsetSampling(quint8 ucChannelId);
    void SetStartOffsetSampling(quint8 ucChannelId, quint64 num);
    void CheckBlockCompress(quint8 ucChannelId,quint16 ullNodeIndex,quint32 ullBlockIndex);
    bool CheckCompress(bool current,quint8* lbp, quint64 len);
    DataEnd GetDataEnd(quint64 ullStartSample, quint8 ucChannelId, bool isBackward);
    qint64 GetSamplingFrequency();
    void SetSamplingFrequency(qint64 ullSamplingFrequency);
    quint32 GetMultiply();
    bool checkChannelId(quint8 ucChannelId);
    Segment* Clone();
    void appendCite();
    void lessenCite();
    bool isCite();

public:
    static const quint32 m_NODE_DATA_SIZE = 64;	
    static const quint32 m_BLOCK_MAX_SIZE = 1024 * 1024;	
    static const quint32 m_BLOCK_NODE_SIZE = m_BLOCK_MAX_SIZE / 64;	
    struct _Node
    {
        quint64 tog;	
        quint64 value;		
        struct{
            quint64 tog;	
            quint64 value;		
        }lbpNode[m_NODE_DATA_SIZE];  
        void* lbp[m_NODE_DATA_SIZE];		
    };
    quint64 m_SamplingDepth;
    quint64 m_TriggerSamplingDepth;
    QVector<quint64> m_maxSampling;
    QVector<bool> m_isFirst;
    QVector<bool> isData;
    bool m_isSave=false;
    bool m_enableGlitchRemoval=false;
    bool m_isMemoryError=false;

private:
    static const quint64 LevelMask[2];
    static const quint64 LevelOffset[2];
    static const quint32 _m_CHUNK_DATA_SIZE = 20480 / m_NODE_DATA_SIZE;	

public:
    QVector<_Node>* GetChannelDataList(qint32 channelID);

private:
    QVector<quint16> _m_ChannelGlitchRemoval;	
    QVector<QVector<_Node>> _m_DataChunks;	
    QVector<quint64> _m_startOffsetSampling;
    qint64 _m_SamplingFrequency=0;
    quint8 _m_ChannelNum=0;		
    quint32 _m_Multiply=0;     
    mutable QMutex _m_dataMutex;
    quint32 _m_cite=0;
    mutable QMutex _m_citeMutex;
};

#endif 

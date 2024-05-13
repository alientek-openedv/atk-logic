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

    /// <summary>
    /// 初始化
    /// </summary>
    /// <param name="channelNum">通道数量</param>
    Segment(quint8 channelNum);

    ~Segment();

    /// <summary>
    /// 获取通道数
    /// </summary>
    quint8 GetChannelNum();

    /// <summary>
    /// 添加通道
    /// </summary>
    void AddChannel();

    /// <summary>
    /// 设置通道毛刺过滤
    /// </summary>
    /// <param name="ucChannelId">通道ID</param>
    /// <param name="ulNum">设置数值</param>
    void SetChannelGlitchRemoval(quint8 ucChannelId, quint32 ulNum);

    /// <summary>
    /// 获取通道毛刺过滤
    /// </summary>
    /// <param name="ucChannelId">通道ID</param>
    quint16 GetChannelGlitchRemoval(quint8 ucChannelId);

    /// <summary>
    /// 设置样本数据
    /// </summary>
    /// <param name="ucChannelId">通道ID</param>
    /// <param name="ullStartPosition">字节起始</param>
    /// <param name="data">数据</param>
    /// <param name="len">数据长度</param>
    /// <param name="isCheckBlockCompress">是否检查压缩</param>
    /// <returns>处理结果</returns>
    SEGMENT_MSG SetSampleBlock(quint8 ucChannelId, quint64 ullStartPosition, quint8* data, quint32 len, bool isCheckBlockCompress);

    /// <summary>
    /// 获取样本块，不计算tog和value
    /// </summary>
    /// <param name="ullStartSample">样本起始位置，必须是8的倍数</param>
    /// <param name="ullEndSample">回调获取结束位置</param>
    /// <param name="ucChannelId">通道ID</param>
    /// <returns>样本起始位置指针</returns>
    quint8* GetSampleBlock(quint64 ullStartSample, quint64& ullEndSample, quint8 ucChannelId);

    /// <summary>
    /// 获取单个样本，计算tog和value
    /// </summary>
    /// <param name="ullStartSample">单个样本</param>
    /// <param name="ucChannelId">通道id</param>
    /// <param name="appendOffset">添加偏移</param>
    /// <returns>单个样本</returns>
    bool GetSample(quint64 ullStartSample, quint8 ucChannelId, bool appendOffset=true);

    /// <summary>
    /// 设置样本数据，不支持修改top和value
    /// </summary>
    /// <param name="ullStartSample">样本起始位置</param>
    /// <param name="ullEndSample">样本结束位置</param>
    /// <param name="isHigh">是否高电平</param>
    /// <param name="ucChannelId">通道id</param>
    void SetSampleBlock(quint64 ullStartSample, quint64 ullEndSample, bool isHigh, quint8 ucChannelId, bool appendOffset);

    /// <summary>
    /// 返回最大样本位置
    /// </summary>
    /// <param name="ucChannelId">通道id</param>
    /// <param name="appendOffset">添加偏移</param>
    /// <returns></returns>
    quint64 GetMaxSample(quint8 ucChannelId, bool appendOffset=true);

    /// <summary>
    /// 返回最大样本位置
    /// </summary>
    /// <param name="ullMaxSample">长度</param>
    /// <param name="ucChannelId">通道id</param>
    /// <param name="appendOffset">添加偏移</param>
    /// <returns></returns>
    void SetMaxSample(quint64 ullMaxSample,quint8 ucChannelId, bool appendOffset=true);

    /// <summary>
    /// 返回起始数据偏移
    /// </summary>
    /// <param name="ucChannelId">通道id</param>
    /// <returns></returns>
    quint64 GetStartOffsetSampling(quint8 ucChannelId);


    /// <summary>
    /// 设置起始数据偏移
    /// </summary>
    /// <param name="ucChannelId">通道id</param>
    /// <returns></returns>
    void SetStartOffsetSampling(quint8 ucChannelId, quint64 num);

    void CheckBlockCompress(quint8 ucChannelId,quint16 ullNodeIndex,quint32 ullBlockIndex);

    bool CheckCompress(bool current,quint8* lbp, quint64 len);

    DataEnd GetDataEnd(quint64 ullStartSample, quint8 ucChannelId, bool isBackward);

    qint64 GetSamplingFrequency();
    void SetSamplingFrequency(qint64 ullSamplingFrequency);//单位KHz
    quint32 GetMultiply();
    bool checkChannelId(quint8 ucChannelId);
    Segment* Clone();

    void appendCite();
    void lessenCite();
    bool isCite();

public:
    static const quint32 m_NODE_DATA_SIZE = 64;	/* 节点数据大小 */
    static const quint32 m_BLOCK_MAX_SIZE = 1024 * 1024;	/* 每块数据1M */
    static const quint32 m_BLOCK_NODE_SIZE = m_BLOCK_MAX_SIZE / 64;	/* 每块数据的分区数量 */
    struct _Node
    {
        quint64 tog;	/* 有整块一样数据的位置 */
        quint64 value;		/* 对应位置的数据 */
        struct{
            quint64 tog;	/* 有整块一样数据的位置 */
            quint64 value;		/* 对应位置的数据 */
        }lbpNode[m_NODE_DATA_SIZE];  /* 每个Node的标记 */
        void* lbp[m_NODE_DATA_SIZE];		/* 每个节点64M数据 */
    };
    quint64 m_SamplingDepth;//采样深度
    quint64 m_TriggerSamplingDepth;//触发位置深度
    QVector<quint64> m_maxSampling;//最大数据位置
    QVector<bool> m_isFirst;
    QVector<bool> isData;
    bool m_isSave=false;//false=未保存
    bool m_enableGlitchRemoval=false;//是否启用
    bool m_isMemoryError=false;//是否内存错误
    QDateTime m_collectDate;//采集时间
    QDateTime m_triggerDate;//采集时间

private:
    static const quint64 LevelMask[2];
    static const quint64 LevelOffset[2];
    static const quint32 _m_CHUNK_DATA_SIZE = 20480 / m_NODE_DATA_SIZE;	/* 数据块大小，20G数据 */

public:
    QVector<_Node>* GetChannelDataList(qint32 channelID);

private:
    QVector<quint16> _m_ChannelGlitchRemoval;	/* 毛刺过滤数组队列，0=不启用 */
    QVector<QVector<_Node>> _m_DataChunks;	/* 样本集合队列 */
    QVector<quint64> _m_startOffsetSampling;//数据偏移
    qint64 _m_SamplingFrequency=0;/* 采样频率kHz */
    quint8 _m_ChannelNum=0;		/* 记录通道数 */
    quint32 _m_Multiply=0;     /* 每个标准样本占多少ns，设置采样率时自动修改，切勿手动修改 */
    mutable QMutex _m_dataMutex;
    quint32 _m_cite=0;//引用
    mutable QMutex _m_citeMutex;
};

#endif // SEGMENT_H

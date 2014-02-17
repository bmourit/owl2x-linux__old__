/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************/


#include "rt_config.h"

#ifdef CONFIG_MULTI_CHANNEL


VOID RtmpPrepareHwNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN BOOLEAN bQosNull,
	IN BOOLEAN bEOSP,
	IN UCHAR OldUP,
	IN UCHAR OpMode,
	IN UCHAR PwrMgmt,
	IN BOOLEAN bWaitACK,
	IN CHAR Index)
{
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	TXWI_STRUC *pTxWI;
	TXINFO_STRUC *pTxInfo;
	PUCHAR pNullFrame;
	PHEADER_802_11 pNullFr;
	UINT32 frameLen;
	UINT32 totalLen;
	UCHAR *ptr;
	UINT i;
	UINT32 longValue;
	UCHAR MlmeRate;

#ifdef RT_BIG_ENDIAN
	NDIS_STATUS    NState;
	PUCHAR pNullFrBuf;
#endif /* RT_BIG_ENDIAN */


	NdisZeroMemory(pAd->NullFrBuf, sizeof(pAd->NullFrBuf));
	pTxWI = (TXWI_STRUC *)&pAd->NullFrBuf[0];
	pNullFrame = &pAd->NullFrBuf[TXWISize];

	pNullFr = (PHEADER_802_11) pNullFrame;
	frameLen = sizeof(HEADER_802_11);
	
	pNullFr->FC.Type = BTYPE_DATA;
	pNullFr->FC.SubType = SUBTYPE_NULL_FUNC;
	pNullFr->FC.ToDs = 1;
	pNullFr->FC.FrDs = 0;

	COPY_MAC_ADDR(pNullFr->Addr1, pEntry->Addr);
	{
		COPY_MAC_ADDR(pNullFr->Addr2, pAd->CurrentAddress);
		COPY_MAC_ADDR(pNullFr->Addr3, pAd->CommonCfg.Bssid);
	}

	pNullFr->FC.PwrMgmt = PwrMgmt;

	pNullFr->Duration = pAd->CommonCfg.Dsifs + RTMPCalcDuration(pAd, pAd->CommonCfg.TxRate, 14);

	/* sequence is increased in MlmeHardTx */
	pNullFr->Sequence = pAd->Sequence;
	pAd->Sequence = (pAd->Sequence+1) & MAXSEQ; /* next sequence  */

	if (bQosNull)
	{
		UCHAR *qos_p = ((UCHAR *)pNullFr) + frameLen;

		pNullFr->FC.SubType = SUBTYPE_QOS_NULL;

		/* copy QOS control bytes */
		qos_p[0] = ((bEOSP) ? (1 << 4) : 0) | OldUP;
		qos_p[1] = 0;
		frameLen += 2;
	} /* End of if */

	RTMPWriteTxWI(pAd, pTxWI,  FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, 0, pEntry->Aid, frameLen,
		0, 0, (UCHAR)pAd->CommonCfg.MlmeTransmit.field.MCS, IFS_HTTXOP, FALSE, &pAd->CommonCfg.MlmeTransmit);

	dumpTxWI(pAd, pTxWI);

	if (bWaitACK)
		pTxWI->TxWITXRPT = 1;

	hex_dump("RtmpPrepareHwNullFrame", pAd->NullFrBuf, TXWISize + frameLen);

	totalLen = TXWISize + frameLen;
	pAd->NullFrLen = totalLen;
	ptr = pAd->NullFrBuf;

#ifdef RT_BIG_ENDIAN
	NState = os_alloc_mem(pAd, (PUCHAR *) &pNullFrBuf, 100);
	if ( NState == NDIS_STATUS_FAILURE )
		return;

	NdisZeroMemory(pNullFrame, 100);
	NdisMoveMemory(pNullFrBuf, pAd->NullFrBuf, totalLen);
	RTMPWIEndianChange(pAd, pNullFrBuf, TYPE_TXWI);
	RTMPFrameEndianChange(pAd, (PUCHAR)pNullFrBuf + TXWISize, DIR_WRITE, FALSE);

	ptr = pNullFrBuf;
#endif /* RT_BIG_ENDIAN */


	for (i= 0; i< totalLen; i+=4)
	{
		longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
		//hex_dump("null frame before",&longValue, 4);

		if (Index == 0)
			RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[0] + i, longValue);
		else if (Index == 1)
			RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[1] + i, longValue);

		//RTMP_IO_WRITE32(pAd, 0xB700 + i, longValue);
		//RTMP_IO_WRITE32(pAd, 0xB780 + i, longValue);

		ptr += 4;
	}



}


VOID RTMPHwSendNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR TxRate,
	IN BOOLEAN bQosNull,
	IN USHORT PwrMgmt,
	IN CHAR Index)
{

	UINT8 TXWISize = pAd->chipCap.TXWISize;
	NDIS_STATUS    NState;
	PHEADER_802_11 pNullFr;
	UCHAR *ptr;
	UINT32 longValue;
#ifdef RT_BIG_ENDIAN
	PUCHAR pNullFrame;
#endif /* RT_BIG_ENDIAN */


	DBGPRINT(RT_DEBUG_TRACE, ("%s - Send NULL Frame @%d Mbps...\n", __FUNCTION__, RateIdToMbps[pAd->CommonCfg.TxRate]));

	pNullFr = (PHEADER_802_11)((&pAd->NullFrBuf[0]) +TXWISize);

	pNullFr->FC.PwrMgmt = PwrMgmt;

	pNullFr->Duration = pAd->CommonCfg.Dsifs + RTMPCalcDuration(pAd, TxRate, 14);

	/* sequence is increased in MlmeHardTx */
	pNullFr->Sequence = pAd->Sequence;
	pAd->Sequence = (pAd->Sequence+1) & MAXSEQ; /* next sequence  */

	//hex_dump("RtmpPrepareHwNullFrame", pAd->NullFrBuf,  pAd->NullFrLen);

	if (Index == 0)
	{
		ptr = pAd->NullFrBuf + TXWISize;

#ifdef RT_BIG_ENDIAN
		longValue =  (*ptr << 8) + *(ptr + 1) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
#else
		longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
#endif /* RT_BIG_ENDIAN */
		RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[0] + TXWISize, longValue);

		ptr = pAd->NullFrBuf + TXWISize + 20;	// update Sequence
		longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
		RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[0] + TXWISize + 20, longValue);
	}
	else if (Index == 1)
	{
		ptr = pAd->NullFrBuf + TXWISize;
#ifdef RT_BIG_ENDIAN
		longValue =  (*ptr << 8) + *(ptr + 1) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
#else
		longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
#endif /* RT_BIG_ENDIAN */
		RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[1] + TXWISize, longValue);

		ptr = pAd->NullFrBuf + TXWISize + 20;	// update Sequence
		longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
		RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[1] + TXWISize + 20, longValue);
	}

	RTMP_IO_WRITE32(pAd, PBF_CTRL, 0x04);

}


#endif /* CONFIG_MULTI_CHANNEL */


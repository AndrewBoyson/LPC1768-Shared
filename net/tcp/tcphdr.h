extern void TcpHdrLog(uint16_t calculatedChecksum);

extern void TcpHdrReadFromPacket(void* pPacket);

extern void TcpHdrWriteToPacket(void* pPacket);
extern void TcpHdrSetChecksum(char* pPacket, uint16_t value);

extern void TcpHdrMakeEmpty(void);

extern uint16_t TcpHdrSrcPort;
extern uint16_t TcpHdrDstPort;
extern uint32_t TcpHdrSeqNum;
extern uint32_t TcpHdrAckNum;
extern int      TcpHdrSizeGet(void);
extern bool     TcpHdrURG; //indicates that the Urgent pointer field is significant
extern bool     TcpHdrACK; //indicates that the Acknowledgment field is significant. All packets after the initial SYN packet sent by the client should have this flag set.
extern bool     TcpHdrPSH; //Push function. Asks to push the buffered data to the receiving application.
extern bool     TcpHdrRST; //Reset the connection
extern bool     TcpHdrSYN; //Synchronize sequence numbers. Only the first packet sent from each end should have this flag set. Some other flags and fields change meaning based on this flag, and some are only valid for when it is set, and others when it is clear.
extern bool     TcpHdrFIN; //No more data from sender
extern uint16_t TcpHdrWindow;
extern uint16_t TcpHdrChecksum;
extern uint16_t TcpHdrUrgent;

extern uint16_t TcpHdrMssGet(void);
extern void     TcpHdrMssSet(uint16_t value);

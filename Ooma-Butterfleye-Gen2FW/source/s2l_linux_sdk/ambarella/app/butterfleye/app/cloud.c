#include <stdbool.h>
#include <pthread.h>
#include "butterfleye.h"
#include "log.h"
#include "utils.h"

static bool gbExitThread = false;

//MESSAGE_API( CLOUD, kAPP_CloudThread );

static int sCLOUD_SendMessage( eAPP_ThreadId anReceiver,
        eMSGQ_Message anMessage,
        char *asPayload, size_t anSize) 
{ 
    int nRet; 
    tMessage Message; 
    
    Message.nReceiver = anReceiver; 
    Message.nSender = kAPP_CloudThread; 
    Message.nMessage = anMessage; 
    Message.nPayloadSize 
        = ( anSize > MAX_PAYLOAD_SZ ) ? MAX_PAYLOAD_SZ : anSize; 
    if( anSize > MAX_PAYLOAD_SZ ) 
    { 
        gLOG_Log( kLOG_Warn, "Payload size %d is larger than %d -- truncating\n", 
            anSize, MAX_PAYLOAD_SZ ); 
    } 
    if( asPayload && Message.nPayloadSize ) 
    { 
        memcpy( Message.sPayload, asPayload, Message.nPayloadSize ); 
    }
    
    nRet = gMSGQ_Send( &Message ); 
    
    return nRet; 
} 


static int sCLOUD_ReceiveMessage( eAPP_ThreadId *apnSender, 
        eMSGQ_Message * apnMessage,
        char *asPayload, size_t * apnSize) 
{ 
    int nRet; 
    tMessage Message; 
    
    Message.nReceiver = kAPP_CloudThread; 
    nRet = gMSGQ_Receive( &Message ); 
    if( nRet < 0 ) 
    { 
        return nRet; 
    } 
    
    *apnSender = Message.nSender; 
    *apnMessage = Message.nMessage; 
    *apnSize = Message.nPayloadSize; 
    if( asPayload && Message.nPayloadSize ) 
    { 
        memcpy( asPayload, Message.sPayload, Message.nPayloadSize ); 
    }
    
    return nRet; 
}


static int sCLOUD_Init( void )
{
    return 0;
}

static int sCLOUD_Deinit( void )
{
    return 0;
}

void * gCLOUD_MainLoop( void *arg )
{
    int nRet;

    // Received message
    eAPP_ThreadId nSender;
    eMSGQ_Message nRecvdMessage;
    size_t nRecvdMsgPayloadSize;
    char sRecvdMsgPayload[ MAX_PAYLOAD_SZ ];

    // Initialize
    nRet = sCLOUD_Init();
    if( nRet < 0 )
    {
        gLOG_Log( kLOG_Error, "Cloud initialization failed\n" );
        goto exit;
    }

    gLOG_Log( kLOG_Info, "Cloud thread started\n" );
    sCLOUD_SendMessage(kAPP_EventManagerThread, kMSGQ_StatusThreadStarted, NULL, 0 );

    while( !gbExitThread )
    {
        nRecvdMessage = kMSGQ_CmdNoneStatusNone;
        nRet = sCLOUD_ReceiveMessage( &nSender, &nRecvdMessage,
                sRecvdMsgPayload, &nRecvdMsgPayloadSize );
        if( nRet < 0 )
        {
            gLOG_Log( kLOG_Error, "Cloud thread error receiving message\n" );
            continue;
        }

        switch( nRecvdMessage )
        {
        // Add other cases here
        case kMSGQ_CmdExitThread:
            gbExitThread = true;
            break;

        default:
            gLOG_Log( kLOG_Error, "Cloud thread received unknown message\n" );
            break;
        }
    }

    // Initialize
    nRet = sCLOUD_Deinit();
    if( nRet < 0 )
    {
        gLOG_Log( kLOG_Error, "Cloud de-initialization failed\n" );
    }


    sCLOUD_SendMessage (kAPP_EventManagerThread, kMSGQ_StatusThreadExited, NULL, 0 );

    gLOG_Log( kLOG_Info, "Cloud thread exited\n" );

exit:
    pthread_exit( NULL );
    return 0;
}
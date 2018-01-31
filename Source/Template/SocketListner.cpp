// Fill out your copyright notice in the Description page of Project Settings.

#include "SocketListner.h"
#include "Engine.h"
//#include "openssl/sha.h"

//#include "TimerManager.h"                         
//#include <Iptypes.h>
//#include "Runtime/Engine/Classes/GameFramework/Actor.h"   
//#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
//#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
//#include "Runtime/Networking/Public/Common/TcpSocketBuilder.h"
//
//#include "SocketSubsystem.h"


ASocketListner::ASocketListner()
{
}

ASocketListner::~ASocketListner()
{
}

void ASocketListner::BeginPlay()
{
    Super::BeginPlay();
    //Launch();
    TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateInternetAddr();
    bool isValid;
    RemoteAddress->SetIp( TEXT("192.168.0.50"), isValid );
    RemoteAddress->SetPort( 1234 );
    ListenerSocket = CreateTCPConnectionListener( "OCVSocketListener", "192.168.0.50", 1234 );
    if ( ListenerSocket )
    {
        ListenerSocket->Connect( *RemoteAddress );
    }
   

}

void ASocketListner::Launch()
{
    //IP = 127.0.0.1, Port = 8890 for my Python test case
    if ( !StartTCPReceiver( "OCVSocketListener", "192.168.0.50", 1234 ) )
    {
        ////UE_LOG  "TCP Socket Listener Created!"
        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT( "OCVSocketListener>> Listen socket created!" ) );
        return;
    }

    ////UE_LOG  "TCP Socket Listener Created! Yay!"
}


//void ASocketListner::ListIpAddresses( IpAddresses& ipAddrs )
//{
//    IP_ADAPTER_ADDRESSES* adapter_addresses( NULL );
//    IP_ADAPTER_ADDRESSES* adapter( NULL );
//
//    // Start with a 16 KB buffer and resize if needed -
//    // multiple attempts in case interfaces change while
//    // we are in the middle of querying them.
//    DWORD adapter_addresses_buffer_size = 16 * KB;
//    for ( int attempts = 0; attempts != 3; ++attempts )
//    {
//        adapter_addresses = ( IP_ADAPTER_ADDRESSES* )malloc( adapter_addresses_buffer_size );
//        assert( adapter_addresses );
//
//        DWORD error = ::GetAdaptersAddresses(
//            AF_UNSPEC,
//            GAA_FLAG_SKIP_ANYCAST |
//            GAA_FLAG_SKIP_MULTICAST |
//            GAA_FLAG_SKIP_DNS_SERVER |
//            GAA_FLAG_SKIP_FRIENDLY_NAME,
//            NULL,
//            adapter_addresses,
//            &adapter_addresses_buffer_size );
//
//        if ( ERROR_SUCCESS == error )
//        {
//            // We're done here, people!
//            break;
//        }
//        else if ( ERROR_BUFFER_OVERFLOW == error )
//        {
//            // Try again with the new size
//            free( adapter_addresses );
//            adapter_addresses = NULL;
//
//            continue;
//        }
//        else
//        {
//            // Unexpected error code - log and throw
//            free( adapter_addresses );
//            adapter_addresses = NULL;
//
//            // @todo
//            LOG_AND_THROW_HERE();
//        }
//    }
//
//    // Iterate through all of the adapters
//    for ( adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next )
//    {
//        // Skip loopback adapters
//        if ( IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType )
//        {
//            continue;
//        }
//
//        // Parse all IPv4 and IPv6 addresses
//        for (
//            IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress;
//            NULL != address;
//            address = address->Next )
//        {
//            auto family = address->Address.lpSockaddr->sa_family;
//            if ( AF_INET == family )
//            {
//                // IPv4
//                SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>( address->Address.lpSockaddr );
//
//                char str_buffer[INET_ADDRSTRLEN] = { 0 };
//                inet_ntop( AF_INET, &( ipv4->sin_addr ), str_buffer, INET_ADDRSTRLEN );
//                ipAddrs.mIpv4.push_back( str_buffer );
//            }
//            else if ( AF_INET6 == family )
//            {
//                // IPv6
//                SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>( address->Address.lpSockaddr );
//
//                char str_buffer[INET6_ADDRSTRLEN] = { 0 };
//                inet_ntop( AF_INET6, &( ipv6->sin6_addr ), str_buffer, INET6_ADDRSTRLEN );
//
//                std::string ipv6_str( str_buffer );
//
//                // Detect and skip non-external addresses
//                bool is_link_local( false );
//                bool is_special_use( false );
//
//                if ( 0 == ipv6_str.find( "fe" ) )
//                {
//                    char c = ipv6_str[2];
//                    if ( c == '8' || c == '9' || c == 'a' || c == 'b' )
//                    {
//                        is_link_local = true;
//                    }
//                }
//                else if ( 0 == ipv6_str.find( "2001:0:" ) )
//                {
//                    is_special_use = true;
//                }
//
//                if ( !( is_link_local || is_special_use ) )
//                {
//                    ipAddrs.mIpv6.push_back( ipv6_str );
//                }
//            }
//            else
//            {
//                // Skip all other types of addresses
//                continue;
//            }
//        }
//    }
//
//    // Cleanup
//    free( adapter_addresses );
//    adapter_addresses = NULL;
//
//    // Cheers!
//}
//

//Rama's Start TCP Receiver
bool ASocketListner::StartTCPReceiver(
    const FString& YourChosenSocketName,
    const FString& TheIP,
    const int32 ThePort
) {
    //Rama's CreateTCPConnectionListener
    ListenerSocket = CreateTCPConnectionListener( YourChosenSocketName, TheIP, ThePort );

    //Not created?
    if ( !ListenerSocket )
    {
        GEngine->AddOnScreenDebugMessage( -1, 15.f, FColor::Red, FString::Printf( TEXT( "StartTCPReceiver>> Listen socket could not be created! ~> %s %d" ), *TheIP, ThePort ) );
        return false;
    }

    //Start the Listener! //thread this eventually
    GetWorldTimerManager().SetTimer( MyConnectionListnerTimerHandle, this, &ASocketListner::TCPConnectionListener, 0.01, true );


    return true;
}
//Format IP String as Number Parts
bool ASocketListner::FormatIP4ToNumber( const FString& TheIP, uint8( &Out )[4] )
{
    //IP Formatting
    TheIP.Replace( TEXT( " " ), TEXT( "" ) );

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //						   IP 4 Parts

    //String Parts
    TArray<FString> Parts;
    TheIP.ParseIntoArray( Parts, TEXT( "." ), true );
    if ( Parts.Num() != 4 )
        return false;

    //String to Number Parts
    for ( int32 i = 0; i < 4; ++i )
    {
        Out[i] = FCString::Atoi( *Parts[i] );
    }

    return true;
}

//Rama's Create TCP Connection Listener
FSocket* ASocketListner::CreateTCPConnectionListener( const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize )
{
    uint8 IP4Nums[4];
    if ( !FormatIP4ToNumber( TheIP, IP4Nums ) )
    {
        //        UE_LOG( "Invalid IP! Expecting 4 parts separated by ." );
        return false;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //Create Socket
    FIPv4Endpoint Endpoint( FIPv4Address( IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3] ), ThePort );
    FSocket* ListenSocket = FTcpSocketBuilder( *YourChosenSocketName )
        .AsReusable()
        .BoundToEndpoint( Endpoint )
        .Listening( 500 );
    if ( ListenSocket )
    {
        //Set Buffer Size
        int32 NewSize = 0;
        ListenSocket->SetReceiveBufferSize( ReceiveBufferSize, NewSize );
    }
    //Done!
    return ListenSocket;
}

////Rama's TCP Connection Listener
void ASocketListner::TCPConnectionListener()
{
    //~~~~~~~~~~~~~
    if ( !ListenerSocket ) return;
    //~~~~~~~~~~~~~

    //Remote address
    TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateInternetAddr();
    bool Pending;

    // handle incoming connections
    if ( ListenerSocket->HasPendingConnection( Pending ) && Pending )
    {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //Already have a Connection? destroy previous
        if ( ConnectionSocket )
        {
            ConnectionSocket->Close();
            ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( ConnectionSocket );
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        //New Connection receive!
        ConnectionSocket = ListenerSocket->Accept( *RemoteAddress, TEXT( "RamaTCP Received Socket Connection" ) );

        
        if ( ConnectionSocket != NULL )
        {
            //Global cache of current Remote Address
            RemoteAddressForConnection = FIPv4Endpoint( RemoteAddress );

            // UE_LOG ("Accepted Connection! WOOOHOOOO!!!");
            GEngine->AddOnScreenDebugMessage( -1, 15.f, FColor::Red, TEXT( "Connexion faite youhou " ) );

           // ConnectionSocket->Connect( *RemoteAddress );

            //can thread this too
            GetWorldTimerManager().SetTimer( MySocketListenerTimerHandle, this, &ASocketListner::TCPSocketListener, 0.01, true );
        }
    }
}

//Rama's String From Binary Array
FString ASocketListner::StringFromBinaryArray( TArray<uint8> BinaryArray )
{
    BinaryArray.Add( 0 ); // Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
                          // Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
                          // Use UTF8_TO_TCHAR if needed.
                          // If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
                          // Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
    return FString( ANSI_TO_TCHAR( reinterpret_cast< const char* >( BinaryArray.GetData() ) ) );
}

void ASocketListner::TCPSend( FString ToSend ) {
    ToSend = ToSend + LINE_TERMINATOR; //For Matlab we need a defined line break (fscanf function) "\n" ist not working, therefore use the LINE_TERMINATOR macro form UE

    TCHAR *SerializedChar = ToSend.GetCharArray().GetData();
    int32 Size = FCString::Strlen( SerializedChar );
    int32 Sent = 0;
    uint8* ResultChars = ( uint8* )TCHAR_TO_UTF8( SerializedChar );

    if ( !ConnectionSocket->Send( ResultChars, Size, Sent ) ) {
        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, FString::Printf( TEXT( "Error sending message" ) ) );
    }

}

//Rama's TCP Socket Listener
void ASocketListner::TCPSocketListener()
{
    //~~~~~~~~~~~~~
    if ( !ConnectionSocket ) return;
    //~~~~~~~~~~~~~


    //Binary Array!
    TArray<uint8> ReceivedData;

    uint32 Size;
    while ( ConnectionSocket->HasPendingData( Size ) )
    {
        ReceivedData.Init( FMath::Min( Size, 65507u ), Size );

        int32 Read = 0;
        ConnectionSocket->Recv( ReceivedData.GetData(), ReceivedData.Num(), Read );

        GEngine->AddOnScreenDebugMessage( -1, 15.f, FColor::Red, FString::Printf( TEXT( "Data Read! %d" ), ReceivedData.Num() ) );
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if ( ReceivedData.Num() <= 0 )
    {
        //No Data Received
        return;
    }

    //UE_LOG( "Total Data read!", ReceivedData.Num() );
    GEngine->AddOnScreenDebugMessage( -1, 15.f, FColor::Red, FString::Printf( TEXT( "Data Bytes Read ~> %d" ), ReceivedData.Num() ) );


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //						Rama's String From Binary Array
    const FString ReceivedUE4String = StringFromBinaryArray( ReceivedData );
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    //UE_LOG( "As String!!!!! ~>", ReceivedUE4String );
    GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, FString::Printf( TEXT( "As String Data ~> %s" ), *ReceivedUE4String ) );
    //int Read = 0;
    char* magicKey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    
    TCPSend( FString( "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection : Upgrade\r\nSec - WebSocket - Accept : s3pPLMBiTxaQ9kYGzzhZRbK + xOo = \r\n" ) );
}






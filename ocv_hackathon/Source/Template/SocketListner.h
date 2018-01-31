// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Runtime/Networking/Public/Networking.h" 
////#include <winsock2.h>
#include "Sockets.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketListner.generated.h"



/**
 * 
 */
UCLASS()

class TEMPLATE_API ASocketListner : public AActor
{
    GENERATED_BODY()


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    FSocket* ListenerSocket;
    FSocket* ConnectionSocket;
    FIPv4Endpoint RemoteAddressForConnection;

public:
    bool StartTCPReceiver(
        const FString& YourChosenSocketName,
        const FString& TheIP,
        const int32 ThePort
    );

    FSocket* CreateTCPConnectionListener(
        const FString& YourChosenSocketName,
        const FString& TheIP,
        const int32 ThePort,
        const int32 ReceiveBufferSize = 2 * 1024 * 1024
    );

    void TCPSend( FString ToSend );


    bool FormatIP4ToNumber( const FString& TheIP, uint8( &Out )[4] );
    FString StringFromBinaryArray( TArray<uint8> BinaryArray );

    //Timer functions, could be threads
    void TCPConnectionListener(); 	//can thread this eventually
    void TCPSocketListener();		//can thread this eventually

    void Launch();
public:
	ASocketListner();
	~ASocketListner();

private:
    //void ListIpAddresses( IpAddresses& ipAddrs );

    FTimerHandle MyConnectionListnerTimerHandle;
    FTimerHandle MySocketListenerTimerHandle;
};

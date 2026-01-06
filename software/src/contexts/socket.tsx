'use client';

import React, {
  createContext,
  useContext,
  useEffect,
  useState,
  PropsWithChildren,
  FC,
} from 'react';
import { io, Socket } from 'socket.io-client';

const SOCKET_MESSAGES_MAX = 100;

export type SocketContextType = {
  connected: boolean;
  socket: Socket | null;
  sockets: string[]; // List of connected sockets
  messages: { [key: string]: object[] };
  // clearMessages?: (label: string) => void;
};

const SocketContext = createContext<SocketContextType>({
  connected: false,
  socket: null,
  sockets: [],
  messages: {},
});

export const useSocket = () => {
  const context = useContext(SocketContext);
  if (!context) {
    throw new Error('useSocket must be used within a DebugSocketProvider');
  }
  return context;
};

// const socket = io({ autoConnect: false });
const socket = io();

const DebugSocketProvider: FC<PropsWithChildren> = ({ children }) => {
  const [isConnected, setIsConnected] = useState(socket.connected);
  const [sockets, setSockets] = useState<{ [key: string]: boolean }>({});
  const [messages, setMessages] = useState<{ [key: string]: object[] }>({});

  // const clearMessages = (label: string) => {
  //   setMessages((prev) => {
  //     const updatedMessages = { ...prev, [label]: [] };
  //     return updatedMessages;
  //   });
  // };

  useEffect(() => {
    const addMessage = (label: string, msg: object) => {
      setMessages((prev) => {
        // console.log(prev);
        return {
          ...prev,
          [label]: [msg, ...(prev[label] ?? [])].slice(0, SOCKET_MESSAGES_MAX),
        };
      });
    };

    function onJson(socketMsg: object) {
      // Subsocket handling
      if (typeof socketMsg['_socket'] === 'string') {
        setSockets((prev) => {
          if (!prev[socketMsg['_socket']]) {
            console.log(`New Socket: ${socketMsg['_socket']}`);
          }
          return { ...prev, [socketMsg['_socket']]: true };
        });
      }
      addMessage('server', socketMsg);
    }

    const onConnect = () => {
      console.log(`Socket Connected: ${socket.id}`);
      setIsConnected(true);
      // socket.on('json', onJson);
    };
    const onDisconnect = (rsn: string) => {
      console.log(`Socket Disconnected: ${rsn}`);
      setIsConnected(false);
      // socket.off('json', onJson);
      // setSockets({});
      // setMessages({});
    };

    setIsConnected(socket.connected);

    socket.on('connect', onConnect);
    socket.on('disconnect', onDisconnect);
    socket.on('json', onJson);
    Object.entries(sockets).forEach((s) => {
      if (s[1]) {
        socket.on(s[0], (msg: object) => {
          addMessage(s[0], msg);
        });
      }
    });

    return () => {
      socket.off('connect', onConnect);
      socket.off('disconnect', onDisconnect);
      socket.off('json', onJson);
      Object.keys(sockets).forEach((s) => socket.removeAllListeners(s));
    };
  });
  // }, [sockets]);

  const listSockets = (): string[] =>
    Object.entries(sockets).reduce((arr, s) => {
      if (s[1]) {
        arr.push(s[0]);
      }
      return arr;
    }, [] as string[]);

  return (
    <SocketContext.Provider
      value={{
        connected: isConnected,
        socket: isConnected ? socket : null,
        sockets: isConnected ? listSockets() : [],
        messages,
      }}
    >
      {children}
    </SocketContext.Provider>
  );
};

export default DebugSocketProvider;

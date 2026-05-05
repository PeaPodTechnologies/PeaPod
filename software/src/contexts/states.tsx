import React, {
  createContext,
  useContext,
  useState,
  ReactNode,
  useEffect,
  useMemo,
} from 'react';
import { useSocket } from './socket';
import { DeviceID } from '../devicetypes';

type StateTree = {
  [key: string]: number | boolean;
};

type StatesContextType = {
  states: StateTree;
  rebuild: (cb?: (response: { error?: string }) => void) => void;
};

const StatesContext = createContext<StatesContextType>({
  states: {},
  rebuild: () => {},
});

const StatesProvider = ({
  children,
  sock,
}: {
  children: ReactNode;
  sock?: string;
}) => {
  const [states, setStates] = useState<StateTree>({});

  const { messages, socket } = useSocket();

  const table = useMemo(() => {
    const topic = sock ?? 'microcontroller';
    const feed = messages?.[topic];
    // if (feed) console.log(feed.filter((msg) => msg['type'] === 'config'));
    return feed
      ? feed
          .filter((msg) => msg['type'] === 'config')
          .sort((a, b) => a['timestamp'] - b['timestamp'])
          .reduce((acc, msg) => ({ ...acc, ...msg['data'] }), {})
      : null;
  }, [messages, sock]);

  useEffect(() => {
    if (messages && table) {
      setStates(table as StateTree);
    }
  }, [table, messages]);

  const handleRebuild = (cb?: (response: { error?: string }) => void) => {
    if (!socket) return;
    const instruction = {
      type: 'config',
      data: {
        list: null,
      },
    };
    socket.emit('serialinput', instruction, cb);
  };

  return (
    <StatesContext.Provider value={{ states, rebuild: handleRebuild }}>
      {children}
    </StatesContext.Provider>
  );
};

export const useStates = () => {
  const context = useContext(StatesContext);
  if (!context) {
    throw new Error('useStates must be used within a StatesProvider');
  }
  return context;
};

export default StatesProvider;

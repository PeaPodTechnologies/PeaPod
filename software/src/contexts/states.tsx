import React, {
  createContext,
  useContext,
  useState,
  ReactNode,
  useEffect,
  useMemo,
} from 'react';
import { useSocket } from './socket';

type StateTree = {
  [key: string]: number | boolean;
};

type StatesContextType = {
  states: StateTree;
  flags: Record<string, boolean>;
  variables: Record<string, number>;
  rebuild: (cb?: (response: { error?: string }) => void) => void;
};

const StatesContext = createContext<StatesContextType>({
  states: {},
  flags: {},
  variables: {},
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

  const flags = useMemo(() => {
    return Object.fromEntries(
      Object.entries(states).filter(([_, value]) => typeof value === 'boolean')
    ) as Record<string, boolean>;
  }, [states]);

  const variables = useMemo(() => {
    return Object.fromEntries(
      Object.entries(states).filter(([_, value]) => typeof value === 'number')
    ) as Record<string, number>;
  }, [states]);

  return (
    <StatesContext.Provider
      value={{ states, flags, variables, rebuild: handleRebuild }}
    >
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

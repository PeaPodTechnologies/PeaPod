import React, {
  createContext,
  useContext,
  useState,
  ReactNode,
  useEffect,
  useMemo,
} from 'react';
import { useSocket } from './socket';

type IntervalTree = {
  [key: string]: {
    key: string;
    delta: number;
    phase: number;
    flag: string;
    invert: boolean;
  };
};

type IntervalsContextType = {
  intervals: IntervalTree;
  rebuild: (cb?: (response: { error?: string }) => void) => void;
};

const IntervalsContext = createContext<IntervalsContextType>({
  intervals: {},
  rebuild: () => {},
});

const IntervalsProvider = ({
  children,
  sock,
}: {
  children: ReactNode;
  sock?: string;
}) => {
  const [intervals, setIntervals] = useState<IntervalTree>({});

  const { messages, socket } = useSocket();

  const table = useMemo(() => {
    const topic = sock ?? 'microcontroller';
    const feed = messages?.[topic];
    // if (feed) console.log(feed.filter((msg) => msg['type'] === 'config'));
    return feed
      ? feed
          .filter((msg) => msg['type'] === 'interval')
          .sort((a, b) => a['timestamp'] - b['timestamp'])
          .reduce(
            (acc, msg) => ({
              ...acc,
              ...(msg['data'].key ? { [msg['data'].key]: msg['data'] } : {}),
            }),
            {}
          )
      : null;
  }, [messages, sock]);

  useEffect(() => {
    if (messages && table) {
      setIntervals(table as IntervalTree);
    }
  }, [table, messages]);

  const handleRebuild = (cb?: (response: { error?: string }) => void) => {
    if (!socket) return;
    const instruction = {
      type: 'interval',
      data: {
        list: null,
      },
    };
    socket.emit('serialinput', instruction, cb);
  };

  return (
    <IntervalsContext.Provider value={{ intervals, rebuild: handleRebuild }}>
      {children}
    </IntervalsContext.Provider>
  );
};

export const useIntervals = () => {
  const context = useContext(IntervalsContext);
  if (!context) {
    throw new Error('useIntervals must be used within a IntervalsProvider');
  }
  return context;
};

export default IntervalsProvider;

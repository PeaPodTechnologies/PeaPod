import React, {
  createContext,
  useContext,
  useState,
  ReactNode,
  useEffect,
  useMemo,
} from 'react';
import { useSocket } from './socket';

type EventTree = {
  [key: string]: {
    key: string;
    delta: number;
    flag: string;
    invert: boolean;
  };
};

type EventsContextType = {
  events: EventTree;
  rebuild: (cb?: (response: { error?: string }) => void) => void;
};

const EventsContext = createContext<EventsContextType>({
  events: {},
  rebuild: () => {},
});

const EventsProvider = ({
  children,
  sock,
}: {
  children: ReactNode;
  sock?: string;
}) => {
  const [events, setEvents] = useState<EventTree>({});

  const { messages, socket } = useSocket();

  const table = useMemo(() => {
    const topic = sock ?? 'microcontroller';
    const feed = messages?.[topic];
    // if (feed) console.log(feed.filter((msg) => msg['type'] === 'config'));
    return feed
      ? feed
          .filter((msg) => msg['type'] === 'event')
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
      setEvents(table as EventTree);
    }
  }, [table, messages]);

  const handleRebuild = (cb?: (response: { error?: string }) => void) => {
    if (!socket) return;
    const instruction = {
      type: 'event',
      data: {
        list: null,
      },
    };
    socket.emit('serialinput', instruction, cb);
  };

  return (
    <EventsContext.Provider value={{ events, rebuild: handleRebuild }}>
      {children}
    </EventsContext.Provider>
  );
};

export const useEvents = () => {
  const context = useContext(EventsContext);
  if (!context) {
    throw new Error('useEvents must be used within a EventsProvider');
  }
  return context;
};

export default EventsProvider;

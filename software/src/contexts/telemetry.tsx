import React, {
  createContext,
  useContext,
  useState,
  ReactNode,
  useEffect,
} from 'react';
import { useSocket } from './socket';

type TelemetryType = {
  [key: string]: {
    x: number;
    y: number;
  }[];
};

type TelemetryContextType = {
  telemetry: TelemetryType;
};

const TelemetryContext = createContext<TelemetryContextType>({
  telemetry: null,
});

const TelemetryProvider = ({
  children,
  sock,
}: {
  children: ReactNode;
  sock: string;
}) => {
  const [telemetry, setTelemetry] = useState<TelemetryType>({});

  const { messages } = useSocket();

  // TODO: This is insanely inefficient, fix this later - Maybe move to socket context?
  useEffect(() => {
    if (messages && messages[sock]) {
      setTelemetry(
        messages[sock]
          .filter(
            (msg) =>
              msg['type'] === 'event' &&
              msg['data'] &&
              Object.values(msg['data']).every(
                (datum) => typeof datum === 'number'
              )
          )
          .reduce((acc, _msg) => {
            const msg = _msg as {
              timestamp: number;
              data: { [key: string]: number };
            };
            Object.entries(msg['data']).forEach(([key, value]) => {
              if (!acc[key]) {
                acc[key] = [];
              }
              acc[key].push({
                y: value,
                x: msg['timestamp'] || Date.now(),
              });
            });
            return acc;
          }, {} as TelemetryType) as TelemetryType
      );
    }
  }, [messages, sock]);

  return (
    <TelemetryContext.Provider value={{ telemetry }}>
      {children}
    </TelemetryContext.Provider>
  );
};

export const useTelemetry = () => {
  const context = useContext(TelemetryContext);
  if (!context) {
    throw new Error('useTelemetry must be used within a TelemetryProvider');
  }
  return context;
};

export default TelemetryProvider;

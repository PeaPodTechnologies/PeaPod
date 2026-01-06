import React, {
  createContext,
  useContext,
  useState,
  ReactNode,
  useEffect,
} from 'react';
import { useSocket } from './socket';
import { DeviceID } from '../devicetypes';

type DeviceTree = {
  [key in DeviceID]?: number[];
}[];

type DevicesContextType = {
  devices: DeviceTree;
  devicesFlat: [DeviceID, number][];
};

const DevicesContext = createContext<DevicesContextType>({
  devices: [],
  devicesFlat: [],
});

const DevicesProvider = ({
  children,
  sock,
}: {
  children: ReactNode;
  sock?: string;
}) => {
  const [devices, setDevices] = useState<DeviceTree>([]);

  const { messages } = useSocket();

  const tree = messages[sock ?? 'microcontroller']
    ? messages[sock ?? 'microcontroller'].find((msg) => msg['type'] == 'tree')
    : null;

  useEffect(() => {
    if (messages && tree) {
      setDevices(tree['data'] as DeviceTree);
    }
  }, [tree, messages]);

  const devicesFlat = devices.reduce(
    (flat, m) => {
      Object.entries(m).forEach(([id, fqas]) => {
        fqas.forEach((fqa) => {
          flat.push([id as DeviceID, fqa]);
        });
      });
      return flat;
    },
    [] as [DeviceID, number][]
  );

  return (
    <DevicesContext.Provider value={{ devices, devicesFlat }}>
      {children}
    </DevicesContext.Provider>
  );
};

export const useDevices = () => {
  const context = useContext(DevicesContext);
  if (!context) {
    throw new Error('useDevices must be used within a DevicesProvider');
  }
  return context;
};

export default DevicesProvider;

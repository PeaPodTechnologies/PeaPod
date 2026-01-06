'use client';

import React, { FC } from 'react';
import DebugSocketProvider from '@/contexts/socket';
import Dashboard from '../pages/dashboard';
import DevicesProvider from '@/contexts/devices';
import TelemetryProvider from '@/contexts/telemetry';

// eslint-disable-next-line @typescript-eslint/no-unused-vars
const Home: FC = (props) => {
  return (
    <DebugSocketProvider>
      <DevicesProvider sock="microcontroller">
        <TelemetryProvider sock="microcontroller">
          <Dashboard />
        </TelemetryProvider>
      </DevicesProvider>
    </DebugSocketProvider>
  );
};

export default Home;

'use client';

import React, { FC } from 'react';
import DebugSocketProvider from '@/contexts/socket';
import Dashboard from '../pages/dashboard';
import DevicesProvider from '@/contexts/devices';
import TelemetryProvider from '@/contexts/telemetry';
import StatesProvider from '../contexts/states';
import IntervalsProvider from '@/contexts/intervals';
import EventsProvider from '@/contexts/events';

// eslint-disable-next-line @typescript-eslint/no-unused-vars
const Home: FC = (props) => {
  return (
    <DebugSocketProvider>
      <DevicesProvider sock="microcontroller">
        <TelemetryProvider sock="microcontroller">
          <StatesProvider sock="microcontroller">
            <IntervalsProvider sock="microcontroller">
              <EventsProvider sock="microcontroller">
                <Dashboard />
              </EventsProvider>
            </IntervalsProvider>
          </StatesProvider>
        </TelemetryProvider>
      </DevicesProvider>
    </DebugSocketProvider>
  );
};

export default Home;

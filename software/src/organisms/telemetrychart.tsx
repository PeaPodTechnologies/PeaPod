'use client';

import { FC, useState } from 'react';
import {
  Box,
  Button,
  Checkbox,
  Grid,
  List,
  ListItem,
  ListItemText,
  Paper,
  TextField,
  Typography,
} from '@mui/material';
import { useDevices } from '@/contexts/devices';
import { useSocket } from '@/contexts/socket';
import { useTelemetry } from '@/contexts/telemetry';
import { LineChart, ScatterChart } from '@mui/x-charts';

// export type DeviceTreeProps = {
//   socket?: string;
//   // messages?: DebugJsonMessage[];
//   // setNumData?: (n: number) => void;
// };

const TelemetryChart: FC<{
  label: string;
  data: { x: number; y: number }[];
}> = ({ label, data }) => {
  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">{label}</Typography>
      <LineChart
        height={300}
        xAxis={[
          {
            label: 'Time Elapsed (s)',
            data: data.map((point) => point.x / 1000),
          },
        ]}
        series={[
          {
            label: label,
            data: data.map((point) => point.y),
          },
        ]}
        grid={{ vertical: true, horizontal: true }}
        skipAnimation
      />
    </Paper>
  );
};

export default TelemetryChart;

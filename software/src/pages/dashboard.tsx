'use client';

import {
  Grid,
  CircularProgress,
  Box,
  AppBar,
  Divider,
  Drawer,
  List,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Toolbar,
  Typography,
  Checkbox,
} from '@mui/material';
import { FC, PropsWithChildren, useState } from 'react';
import { useSocket } from '@/contexts/socket';
import DebugMessageBoard from '@/organisms/board';
import DeviceTree from '@/organisms/tree';
import Pinger from '@/organisms/ping';
import Device from '@/organisms/device';
import { useTelemetry } from '@/contexts/telemetry';
import TelemetryChart from '@/organisms/telemetrychart';
import { useDevices } from '@/contexts/devices';
import MessageDataGrid from '@/organisms/datagrid';
import {
  Build,
  CalendarMonth,
  Chat,
  DeviceHub,
  Insights,
  Memory,
  Router,
} from '@mui/icons-material';
import Descheduler from '../organisms/descheduler';
import Linker from '../organisms/unlinker';

const drawerWidth = '240px';

const Dashboard: FC<PropsWithChildren> = ({ children }) => {
  const { connected, sockets } = useSocket();
  const { devicesFlat } = useDevices();
  const { telemetry } = useTelemetry();

  const [enableTelemetry, setEnableTelemetry] = useState<boolean>(true);
  const [enableMessages, setEnableMessages] = useState<boolean>(true);
  const [enableDevices, setEnableDevices] = useState<boolean>(true);
  const [enableTree, setEnableTree] = useState<boolean>(true);
  const [enablePinger, setEnablePinger] = useState<boolean>(true);
  const [enableDescheduler, setEnableDescheduler] = useState<boolean>(true);
  const [enableLinker, setEnableLinker] = useState<boolean>(true);

  return (
    <Box sx={{ display: 'flex' }}>
      <AppBar
        position="fixed"
        // sx={{ width: `calc(100% - ${drawerWidth})`, ml: `${drawerWidth}` }}
        sx={{ zIndex: (theme) => theme.zIndex.drawer + 1 }}
      >
        <Toolbar>
          <Typography variant="h6" noWrap component="div">
            I2CIP.js Dashboard
          </Typography>
        </Toolbar>
      </AppBar>
      <Drawer
        sx={{
          width: drawerWidth,
          flexShrink: 0,
          '& .MuiDrawer-paper': {
            width: drawerWidth,
            boxSizing: 'border-box',
          },
        }}
        variant="permanent"
        anchor="left"
      >
        <Toolbar />
        <Divider />
        <List>
          <ListItem disablePadding>
            <ListItemButton
              onClick={() => setEnableTelemetry(!enableTelemetry)}
            >
              <ListItemIcon>
                <Insights />
              </ListItemIcon>
              <ListItemText primary={'Telemetry Charts'} />
              <Checkbox
                edge="end"
                checked={enableTelemetry}
                onChange={() => setEnableTelemetry(!enableTelemetry)}
              />
            </ListItemButton>
          </ListItem>
          <Divider />
          <ListItem disablePadding>
            <ListItemButton onClick={() => setEnableMessages(!enableMessages)}>
              <ListItemIcon>
                <Chat />
              </ListItemIcon>
              <ListItemText primary={'Message Boards'} />
              <Checkbox
                edge="end"
                checked={enableMessages}
                onChange={() => setEnableMessages(!enableMessages)}
              />
            </ListItemButton>
          </ListItem>
          <Divider />
          <ListItem disablePadding>
            <ListItemButton onClick={() => setEnableDevices(!enableDevices)}>
              <ListItemIcon>
                <Memory />
              </ListItemIcon>
              <ListItemText primary={'Device Controls'} />
              <Checkbox
                edge="end"
                checked={enableDevices}
                onChange={() => setEnableDevices(!enableDevices)}
              />
            </ListItemButton>
          </ListItem>
          <Divider />
          <ListItem disablePadding>
            <ListItemButton onClick={() => setEnableTree(!enableTree)}>
              <ListItemIcon>
                <DeviceHub />
              </ListItemIcon>
              <ListItemText primary={'Module Tree'} />
              <Checkbox
                edge="end"
                checked={enableTree}
                onChange={() => setEnableTree(!enableTree)}
              />
            </ListItemButton>
          </ListItem>
          <Divider />
          <ListItem disablePadding>
            <ListItemButton onClick={() => setEnablePinger(!enablePinger)}>
              <ListItemIcon>
                <Router />
              </ListItemIcon>
              <ListItemText primary={'Ping Tool'} />
              <Checkbox
                edge="end"
                checked={enablePinger}
                onChange={() => setEnablePinger(!enablePinger)}
              />
            </ListItemButton>
          </ListItem>
          <Divider />
          <ListItem disablePadding>
            <ListItemButton
              onClick={() => setEnableDescheduler(!enableDescheduler)}
            >
              <ListItemIcon>
                <CalendarMonth />
              </ListItemIcon>
              <ListItemText primary={'Descheduler'} />
              <Checkbox
                edge="end"
                checked={enableDescheduler}
                onChange={() => setEnableDescheduler(!enableDescheduler)}
              />
            </ListItemButton>
          </ListItem>
          <Divider />
          <ListItem disablePadding>
            <ListItemButton onClick={() => setEnableLinker(!enableLinker)}>
              <ListItemIcon>
                <Build />
              </ListItemIcon>
              <ListItemText primary={'Linker'} />
              <Checkbox
                edge="end"
                checked={enableLinker}
                onChange={() => setEnableLinker(!enableLinker)}
              />
            </ListItemButton>
          </ListItem>
        </List>
        <Divider />
      </Drawer>
      <Box component="main" sx={{ flexGrow: 1, p: 3 }}>
        <Toolbar />
        {connected ? (
          <Grid
            container
            spacing={3}
            direction="row"
            sx={{
              justifyContent: 'flex-start',
              alignItems: 'flex-start',
            }}
          >
            {enableTelemetry &&
              Object.entries(telemetry)
                .sort(([a], [b]) => a.localeCompare(b))
                .map(([key, data]) => (
                  <Grid size={{ xs: 12, md: 4 }} key={`grid-telemetry-${key}`}>
                    <TelemetryChart label={key} data={data} />
                  </Grid>
                ))}
            {enableMessages &&
              sockets.map((s) => (
                <Grid key={`grid-socket-${s}`} size={{ xs: 12, md: 8 }}>
                  <DebugMessageBoard
                    socket={s}
                    enableSerialInput={s === 'microcontroller'}
                  />
                </Grid>
              ))}
            <Grid size={{ xs: 12, md: 8 }}>
              <MessageDataGrid socket="microcontroller" />
            </Grid>
            {enableTree && (
              <Grid size={{ xs: 12, md: 4 }}>
                <DeviceTree />
              </Grid>
            )}
            {enablePinger && (
              <Grid size={{ xs: 12, md: 4 }}>
                <Pinger />
              </Grid>
            )}
            {enableDescheduler && (
              <Grid size={{ xs: 12, md: 4 }}>
                <Descheduler />
              </Grid>
            )}
            {enableLinker && (
              <Grid size={{ xs: 12, md: 4 }}>
                <Linker />
              </Grid>
            )}
            {enableDevices &&
              devicesFlat.map(([deviceId, fqa]) => (
                <Grid
                  key={`grid-device-${deviceId}-${fqa}`}
                  size={{ xs: 12, md: 4 }}
                >
                  <Device deviceId={deviceId} fqa={fqa} />
                </Grid>
              ))}
          </Grid>
        ) : (
          <Box
            sx={{
              width: '100vw',
              height: '100vh',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
            }}
          >
            <CircularProgress />
          </Box>
        )}
      </Box>
    </Box>
  );
};

export default Dashboard;

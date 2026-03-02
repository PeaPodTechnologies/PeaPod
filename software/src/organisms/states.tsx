'use client';

import { FC, useState } from 'react';
import {
  Alert,
  Box,
  Button,
  Checkbox,
  List,
  ListItem,
  ListItemText,
  Paper,
  Snackbar,
  SnackbarCloseReason,
  TextField,
  Typography,
} from '@mui/material';
import { useDevices } from '@/contexts/devices';
import { useSocket } from '@/contexts/socket';
import { useStates } from '../contexts/states';

const StateTable: FC = () => {
  const { states } = useStates();
  const { socket } = useSocket();

  const [snackbar, setSnackbar] = useState<boolean>(false);
  const [errorSnackbar, setErrorSnackbar] = useState<boolean>(false);
  const [errorMessage, setErrorMessage] = useState<string>('');

  const closeSnackbar = (
    event: React.SyntheticEvent | Event,
    reason?: SnackbarCloseReason
  ) => {
    if (reason === 'clickaway') {
      return;
    }

    setSnackbar(false);
  };

  const closeErrorSnackbar = (
    event: React.SyntheticEvent | Event,
    reason?: SnackbarCloseReason
  ) => {
    if (reason === 'clickaway') {
      return;
    }

    setErrorSnackbar(false);
  };

  const handleResponse = (response: { error?: string }) => {
    if (response && response.error) {
      setErrorMessage(response.error);
      setErrorSnackbar(true);
    } else {
      setSnackbar(true);
    }
  };

  const handleRebuild = () => {
    if (!socket) return;
    const instruction = {
      type: 'config',
      data: {
        list: null,
      },
    };
    socket.emit('serialinput', instruction, handleResponse);
  };

  return (
    //devices ? (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">State Table</Typography>
      {Object.entries(states).length > 0 ? (
        <List>
          {Object.entries(states).map(([key, value]) => (
            <ListItem key={key}>
              <ListItemText primary={key} secondary={String(value)} />
            </ListItem>
          ))}
        </List>
      ) : (
        <Typography variant="subtitle1">No states found</Typography>
      )}
      <Button variant="text" disabled={!socket} onClick={handleRebuild}>
        REFRESH
      </Button>
      <Snackbar open={snackbar} autoHideDuration={6000} onClose={closeSnackbar}>
        <Alert
          onClose={closeSnackbar}
          severity="success"
          variant="filled"
          sx={{ width: '100%' }}
        >
          Serial input received successfully!
        </Alert>
      </Snackbar>
      <Snackbar
        open={errorSnackbar}
        autoHideDuration={6000}
        onClose={closeErrorSnackbar}
      >
        <Alert
          onClose={closeErrorSnackbar}
          severity="error"
          variant="filled"
          sx={{ width: '100%' }}
        >
          {errorMessage || 'An error occurred while sending serial input.'}
        </Alert>
      </Snackbar>
    </Paper>
  ); // : null;
};

export default StateTable;

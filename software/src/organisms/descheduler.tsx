import {
  SnackbarCloseReason,
  Paper,
  Typography,
  Button,
  Snackbar,
  Alert,
  List,
  ListItem,
  ListItemText,
} from '@mui/material';
import { FC, useEffect, useState } from 'react';
import { useSocket } from '../contexts/socket';

const Descheduler: FC = () => {
  const { socket } = useSocket();
  const [schedule, setSchedule] = useState<string[]>([]);
  const [snackbar, setSnackbar] = useState<boolean>(false);
  const [errorsnackbar, setErrorSnackbar] = useState<boolean>(false);
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

  const getSchedule = () => {
    if (socket) {
      socket.emit('scheduler-get', {}, (data: string[]) => {
        setSchedule(data);
        setSnackbar(true);
      });
    }
  };

  const clearSchedule = (label: string) => {
    if (socket) {
      socket.emit('scheduler-clear', label, (err) => {
        if (err) {
          setErrorMessage(err);
          setErrorSnackbar(true);
        } else {
          setSnackbar(true);
        }
      });
    }
  };

  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">Descheduler</Typography>
      {schedule.length > 0 ? (
        <List>
          {schedule.map((item) => (
            <ListItem key={item}>
              <ListItemText primary={item} />
              <Button
                onClick={() => {
                  clearSchedule(item);
                  getSchedule();
                }}
                disabled={!socket}
              >
                Clear
              </Button>
            </ListItem>
          ))}
        </List>
      ) : (
        <Typography variant="body2" color="textSecondary">
          No scheduled tasks found.
        </Typography>
      )}
      <Button onClick={getSchedule} disabled={!socket}>
        Refresh
      </Button>
      <Snackbar open={snackbar} autoHideDuration={6000} onClose={closeSnackbar}>
        <Alert
          onClose={closeSnackbar}
          severity="success"
          variant="filled"
          sx={{ width: '100%' }}
        >
          Schedule updated successfully!
        </Alert>
      </Snackbar>
      <Snackbar
        open={errorsnackbar}
        autoHideDuration={6000}
        onClose={closeErrorSnackbar}
      >
        <Alert
          onClose={closeErrorSnackbar}
          severity="error"
          variant="filled"
          sx={{ width: '100%' }}
        >
          Scheduler Error: {errorMessage}
        </Alert>
      </Snackbar>
    </Paper>
  );
};

export default Descheduler;

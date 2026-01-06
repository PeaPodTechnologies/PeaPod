import {
  SnackbarCloseReason,
  Paper,
  Typography,
  List,
  ListItem,
  ListItemText,
  Button,
  Snackbar,
  Alert,
  Divider,
  Fab,
  Box,
  Select,
  MenuItem,
} from '@mui/material';
import { FC, useState } from 'react';
import { useSocket } from '../contexts/socket';
import { Add, Close } from '@mui/icons-material';
import { useTelemetry } from '../contexts/telemetry';

type LinkerInstruction = {
  type: 'command';
  data: {
    fqa: number; // FQA (Fully Qualified Address)
    g?: string | number | boolean;
    a?: string | number | boolean;
    s?: string | number | boolean;
    b?: string | number | boolean;
  };
};

type LinkerType = {
  [key: string]: {
    key: 'g' | 'a' | 's' | 'b';
    cast: 'string' | 'number' | 'boolean';
    eval?: string; // Optional evaluation expression
    instruction: LinkerInstruction;
  }[];
};

const Linker: FC = () => {
  const { socket } = useSocket();
  const { telemetry } = useTelemetry();
  const [linker, setLinker] = useState<LinkerType>({});
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

  const getLinker = () => {
    if (socket) {
      socket.emit('linker-get', {}, (data: LinkerType) => {
        setLinker(data);
        setSnackbar(true);
      });
    }
  };

  const clearLinker = (label: string, instruction: LinkerInstruction) => {
    if (socket) {
      socket.emit('linker-clear', label, instruction, (err) => {
        if (err) {
          setErrorMessage(err);
          setErrorSnackbar(true);
        } else {
          setSnackbar(true);
        }
      });
    }
  };

  // This component is a placeholder for the Linker functionality.
  // It can be expanded later to include more complex logic or UI elements.
  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">Linker</Typography>
      {Object.keys(linker).length > 0 ? (
        <List>
          {Object.entries(linker).map(([label, tasks]) => {
            return (
              <>
                <Divider key={`divider-${label}`} />
                {tasks.map((task) => (
                  <ListItem key={`${label}-${JSON.stringify(task)}`}>
                    <ListItemText
                      primary={label}
                      secondary={JSON.stringify(task)}
                    />

                    <Button
                      onClick={() => {
                        clearLinker(label, task.instruction);
                        getLinker();
                      }}
                      disabled={!socket}
                    >
                      Clear
                    </Button>
                  </ListItem>
                ))}
              </>
            );
          })}
        </List>
      ) : (
        <Typography variant="body2" color="textSecondary">
          No linker tasks found.
        </Typography>
      )}
      <Button onClick={getLinker} disabled={!socket}>
        Refresh
      </Button>
      <Snackbar open={snackbar} autoHideDuration={6000} onClose={closeSnackbar}>
        <Alert
          onClose={closeSnackbar}
          severity="success"
          variant="filled"
          sx={{ width: '100%' }}
        >
          Linker updated successfully!
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
          Linker Error: {errorMessage}
        </Alert>
      </Snackbar>
    </Paper>
  );
};

export default Linker;

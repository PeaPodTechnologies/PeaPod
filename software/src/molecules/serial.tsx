import {
  Alert,
  Box,
  Button,
  IconButton,
  Snackbar,
  SnackbarCloseReason,
  TextField,
} from '@mui/material';
import { Close } from '@mui/icons-material';
import { FC, useEffect, useState } from 'react';
import { useSocket } from '@/contexts/socket';

type SerialInputProps = {
  socket?: string;
};

const SerialInput: FC<SerialInputProps> = () => {
  const [inputString, setInputString] = useState('{}');
  const [inputError, setInputError] = useState(false);
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

  useEffect(() => {
    try {
      JSON.parse(inputString);
      setInputError(false);
    } catch {
      setInputError(true);
    }
  }, [inputString]);

  const handleInput = () => {
    if (socket && !inputError) {
      try {
        const data = JSON.parse(inputString);
        socket.emit('serialinput', data, (response) => {
          if (response && response.error) {
            setErrorMessage(`Rejected: ${response.error}`);
            setErrorSnackbar(true);
          } else {
            setSnackbar(true);
          }
        });
        setInputString('{}'); // Reset input after sending
      } catch (error) {
        console.error('Invalid JSON input:', error);
      }
    }
  };

  return (
    <Box
      component="form"
      sx={{
        display: 'flex',
        // alignItems: 'center',
        '& .MuiTextField-root': { m: 1, width: '25ch' },
      }}
      noValidate
      autoComplete="off"
      onSubmit={(e) => {
        e.preventDefault(); // Prevents page refresh
        if (!inputError && socket) {
          handleInput();
        }
      }}
    >
      <TextField
        error={inputError}
        id="serial-input"
        label="Serial Input"
        helperText="Enter JSON string to send to socket"
        value={inputString}
        onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
          setInputString(event.target.value);
        }}
      />
      <Box sx={{ m: 1, marginTop: 2 }}>
        <Button
          variant="contained"
          disabled={inputError || !socket}
          onClick={handleInput}
        >
          Send
        </Button>
      </Box>
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
    </Box>
  );
};

export default SerialInput;

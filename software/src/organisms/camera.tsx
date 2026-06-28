'use client';

import { FC, useState } from 'react';
import {
  Alert,
  Button,
  Paper,
  Snackbar,
  SnackbarCloseReason,
  Typography,
} from '@mui/material';
import { useSocket } from '@/contexts/socket';

const Camera: FC = () => {
  const { socket } = useSocket();

  const [snackbar, setSnackbar] = useState<boolean>(false);
  const [errorSnackbar, setErrorSnackbar] = useState<boolean>(false);
  const [errorMessage, setErrorMessage] = useState<string>('');

  const [image, setImage] = useState<string | null>(null);

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

  const handleResponse = (response: {
    error?: string;
    blob?: ArrayBuffer;
    mime?: string;
  }) => {
    if (response && response.error) {
      setErrorMessage(response.error);
      setErrorSnackbar(true);
    } else {
      setSnackbar(true);

      if (response.blob && response.mime) {
        const bytes = new Uint8Array(response.blob);
        const blob = new Blob([bytes], { type: response.mime });
        const url = URL.createObjectURL(blob);
        setImage(url);
      }
    }
  };

  const handleCapture = () => {
    if (!socket) return;
    socket.emit('camera', {}, handleResponse);
  };

  return (
    //devices ? (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">Camera</Typography>
      {image && <img style={{ marginTop: 10 }} src={image} alt="Captured" />}
      <Button
        sx={{ marginTop: 2 }}
        variant="contained"
        disabled={!socket}
        onClick={handleCapture}
      >
        CAPTURE
      </Button>
      <Snackbar open={snackbar} autoHideDuration={6000} onClose={closeSnackbar}>
        <Alert
          onClose={closeSnackbar}
          severity="success"
          variant="filled"
          sx={{ width: '100%' }}
        >
          Camera capture successful!
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
          {errorMessage || 'An error occurred while capturing the camera.'}
        </Alert>
      </Snackbar>
    </Paper>
  ); // : null;
};

export default Camera;

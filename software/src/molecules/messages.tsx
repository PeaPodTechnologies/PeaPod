'use client';

import { DebugJsonMessage } from '$/types';
import React, {
  FC,
  useEffect,
  // useState,
  // useCallback
} from 'react';
import { useSocket } from '@/contexts/socket';
import DebugMessage from '@/atoms/message';
import { List, ListItem } from '@mui/material';

export type DebugJsonMessageDataType = {
  [key: string]: {
    x: number; // timestamp
    y: string; // value display
    _y: number; // true value
  }[];
};

const DebugSocketMessages: FC<{
  label: string;
  num?: number;
  showJson?: boolean;
}> = (props) => {
  const { messages } = useSocket();
  // const [data, setData] = useState<DebugJsonMessageDataType>({});

  // const addData = useCallback(
  //   (label: string, value: unknown, timestamp?: number) => {
  //     setData((prev) => {
  //       if (!prev[label]) {
  //         console.log(`Adding Data Label: ${label}`);
  //       }
  //       const updatedData = {
  //         ...prev,
  //         [label]: [
  //           ...(prev[label] ?? []),
  //           {
  //             x: timestamp ?? (prev[label] ? prev[label].length : 0),
  //             y:
  //               typeof value === 'boolean'
  //                 ? value === false
  //                   ? '0'
  //                   : '1'
  //                 : typeof value === 'number'
  //                   ? value.toFixed(2)
  //                   : JSON.stringify(value as object),
  //             _y:
  //               typeof value === 'boolean'
  //                 ? value === false
  //                   ? 0
  //                   : 1
  //                 : typeof value === 'number'
  //                   ? value
  //                   : NaN,
  //           },
  //         ].slice(0, props.num ?? 1),
  //       };
  //       return updatedData;
  //     });
  //   },
  //   [props.num]
  // );

  useEffect(() => {
    if (props.label && messages[props.label]) {
      (messages[props.label] as DebugJsonMessage).forEach((msg) => {
        if (msg.data) {
          // addData(props.label, msg.timestamp, msg.data);
          // Object.entries(msg.data).forEach(([key, value]) => {
          //   if (key && value) addData(key as string, value, msg.timestamp);
          // });
        }
      });
    }
  }, [props.label, messages]);

  const nosock =
    !props.label || !messages[props.label] || messages[props.label].length == 0;

  return (
    <>
      {/* 
        className explanation:
        - list-inside: List marker is inside the content flow.
        - text-sm: Small text size.
        - text-center sm:text-left: Center text on small screens, left-align on larger.
        - font-[family-name:var(--font-geist-mono)]: Use Geist Mono font.
        - bg-gray-100: Light gray background.
        - rounded-xl: Extra large rounded corners.
        - p-4: Padding on all sides.
      */}
      {/* <ol className="list-inside text-sm text-center sm:text-left font-[family-name:var(--font-geist-mono)] bg-gray-100 rounded-xl p-4"> */}
      <List>
        {nosock ? (
          <li className="mb-2 display:inline-block margin-top:.5rem">
            <span className="text-xs text-gray-400 display:inline-block">
              DEBUG SOCKET EMPTY
            </span>
          </li>
        ) : (
          <>
            {/* <>
              {Object.entries(data).map(([label, arr], i) => {
                return arr && label ? (
                  <ListItem
                    // className="mb-2 display:inline-block margin-top:.5rem"
                    key={`data-${props.label}-${i}`}
                  >
                    <span className="text-xs text-gray-400 display:inline-block">
                      {`${label.toUpperCase().charAt(0) + label.slice(1)}: ${
                        arr.length == 0
                          ? 'EMPTY'
                          : (
                            arr.reduce((acc, d) => acc + d._y, 0) / arr.length
                          ).toFixed(2)
                      }`}
                    </span>
                    <span className="minWidth:1rem display:inline-block">
                      &nbsp;{' '}
                    </span>
                    <span className="text-xs text-gray-400 display:inline-block">
                      {'['}
                      {arr.map((point, j) => {
                        return (
                          <span
                            key={`data-${props.label}-${i}-${j}`}
                            className="ml-1 text-gray-900 display:inline-block"
                          >
                            {`${point.y}, `}
                          </span>
                        );
                      })}
                      {']'}
                    </span>
                  </ListItem>
                ) : (
                  <></>
                );
              })}
            </> */}
            {messages[props.label].slice(0, props.num ?? 25).map((msg, i) => {
              return (
                <ListItem
                  className="mb-2 display:inline-block margin-top:.5rem"
                  key={`debug-${props.label}-${i}`}
                >
                  <DebugMessage
                    msg={msg}
                    label={props.label}
                    showJson={props.showJson}
                  />
                </ListItem>
              );
            })}
          </>
        )}
      </List>
    </>
  );
};

export default DebugSocketMessages;

import React, { FC } from 'react';
import { DebugJsonMessage, parseType, parseTimestamp } from '$/types';

export type DebugMessageProps = {
  label: string;
  showJson?: boolean;
  msg: DebugJsonMessage;
};

const DebugMessage: FC<DebugMessageProps> = ({ label, showJson, msg }) => {
  return (
    <>
      {showJson ? (
        <span className="text-xs text-gray-600 display:inline-block">
          <code>{JSON.stringify(msg)}</code>
        </span>
      ) : (
        <>
          <span className="text-xs text-gray-400 display:inline-block">
            {`[ ${
              msg.t ? `${msg.t}` : parseType(msg.type).toUpperCase()
            } ${msg.timestamp ? parseTimestamp(msg.timestamp) : 'NOW'} ]`}
          </span>
          <span className="minWidth:1rem display:inline-block">&nbsp; </span>
          <span className="text-xs text-gray-400 display:inline-block">
            {msg.data
              ? Object.entries(msg.data).map(([key, value], i) => (
                  <span
                    key={`debug-${label}-message-${i}`}
                    className="ml-1 text-gray-900 display:inline-block"
                  >
                    {key
                      .split('-')
                      .map(
                        (word) =>
                          word.slice(0, 1).toUpperCase() +
                          word.slice(1).toLowerCase()
                      )
                      .join(' ')}
                    : {JSON.stringify(value)}
                  </span>
                ))
              : null}
          </span>
          <span className="text-xs text-gray-600 display:inline-block">
            <code>{JSON.stringify(msg.msg)}</code>
          </span>
        </>
      )}
    </>
  );
};

export default DebugMessage;

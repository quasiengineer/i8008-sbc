/* eslint-disable no-console */

import * as path from 'node:path';
import * as fs from 'node:fs';
import { fileURLToPath } from 'node:url';

import { SerialPort } from 'serialport';
import highwayhash from 'highwayhash';

import { compile } from 'i8008-asm';
import { preprocessFile } from 'i8008-preprocess';

const args = process.argv.slice(2);
const argsMap = Object.fromEntries(args.map((arg) => arg.replace(/^-/, '').split('=')));

// just 0's
const hashKey = Buffer.alloc(32);

const BAUD_RATE = 115200;
const DEFAULT_PORT_NAME = 'COM5';

const InCommandType = Object.freeze({ CmdOutput: 0x00, CmdFinish: 0x01 });

/*
 * Send data to port and wait ack
 */
const sendMemoryImageToDevice = (port, memory) => new Promise((resolve, reject) => (
  port.write(
      Buffer.concat([Buffer.from([memory.length & 0xFF, memory.length >> 8]), memory]),
      (err) => err && reject(err),
  )
));

/*
 * Process received data
 */
const processIncomingData = (port) => {
  let currentBuf = Buffer.alloc(0);

  port.on('data', async (data) => {
    currentBuf = Buffer.concat([currentBuf, data]);

    let dataLen = currentBuf.readUInt16LE(1);
    while (dataLen + 3 <= currentBuf.length) {
      const cmd = currentBuf[0];
      switch (cmd) {
        case InCommandType.CmdFinish:
          process.exit(0);
          break;

        case InCommandType.CmdOutput: {
          const dataStr = currentBuf.subarray(3, 3 + dataLen).toString();
          process.stdout.write(`[${new Date().toISOString()}] ${dataStr}${dataStr.endsWith('\n') ? '' : '\n'}`);
          break;
        }

        default:
          console.log('Unknown response!', cmd);
          process.exit(1);
      }

      currentBuf = currentBuf.subarray(3 + dataLen);
      if (currentBuf.length >= 3) {
        dataLen = currentBuf.readUInt16LE(1);
      }
    }
  });
};

/*
 * Open port
 */
const openPort = (portName) => new Promise((resolve, reject) => {
  const port = new SerialPort({ baudRate: BAUD_RATE, path: portName || DEFAULT_PORT_NAME }, (openErr) => {
    if (openErr) {
      reject(openErr);
      return;
    }

    port.on('error', (err) => console.log('Error: ', err.message || err));

    processIncomingData(port);

    resolve(port);
  });
});

/*
 * Function that either reads binary ROMs or compiles ASM source code into binary
 */
const getBlobWithROMs = (filePath) => {
  const fileData = fs.readFileSync(filePath);
  if (filePath.endsWith('.bin')) {
    console.log('Using binary ROM');
    return fileData;
  }

  const sourceCode = preprocessFile(filePath);

  const hash = highwayhash.asHexString(hashKey, Buffer.from(sourceCode, 'utf-8'));
  const hashKeyFileName = filePath.replace(/\.(.*)$/, '.hash');
  const binaryRomFileName = filePath.replace(/\.(.*)$/, '.bin');
  if (fs.existsSync(hashKeyFileName) && fs.existsSync(binaryRomFileName)) {
    const existingHash = fs.readFileSync(hashKeyFileName, 'utf-8');
    if (existingHash === hash) {
      console.log('Source code has not changed from last run, using existing ROM.');
      return fs.readFileSync(binaryRomFileName);
    }
  }

  console.log('Recompile source code...');
  const { bytecode, errors } = compile(sourceCode);
  if (errors?.length) {
    console.log('COULD NOT PARSE SOURCE CODE!');
    console.log(errors);
    process.exit(1);
  }

  const memory = new Uint8Array(2 ** 14);
  memory.set(bytecode, 0);

  const rom = Buffer.from(memory);
  fs.writeFileSync(hashKeyFileName, hash);
  fs.writeFileSync(binaryRomFileName, rom);
  return rom;
};

const main = async () => {
  const port = await openPort(argsMap.port);
  const dirName = path.dirname(fileURLToPath(import.meta.url));
  await sendMemoryImageToDevice(port, getBlobWithROMs(path.resolve(dirName, argsMap.rom)));
};

main()
  .catch((err) => {
    console.error('Error', err.message, err);
    process.exit();
  });

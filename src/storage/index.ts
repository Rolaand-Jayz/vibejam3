export {
  InMemoryProjectStorage,
  LocalStorageProjectStorage,
  type ProjectStorage,
} from './project-storage.js';

// SqliteProjectStorage is available for Node.js only.
// Import directly from './sqlite-project-storage.js' in Node.js contexts (tests, CLI).
// Do NOT re-export here because it pulls in native modules that break browser builds.
